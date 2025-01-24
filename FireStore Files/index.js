const { https } = require('firebase-functions');  // Fix: Use https from firebase-functions
const functions = require('firebase-functions');  // Firebase functions (only required once)
const admin = require('firebase-admin');
const express = require('express');
const { Storage } = require("@google-cloud/storage");
const { stringify } = require("csv-stringify/sync");
const cors = require('cors')({ origin: true });

admin.initializeApp();
const db = admin.firestore();
const app = express();
app.use(express.json());

app.post('/addSamples', async (req, res) => {
    try {
        const {timestamp,sensorID,sensorModel,unitMac,samples } = req.body;

        const collectionName = `Sampling_${timestamp.replace(/[:\-TZ]/g, '_')}`;
        const collectionRef = db.collection(collectionName);

        const batch = db.batch();
        samples.forEach(({ sample, sample_index }) => {
            const docRef = collectionRef.doc();
            batch.set(docRef, {
                timestamp,
                sensorID,
                sensorModel,
                unitMac,
                sample,
                sample_index,
            });
        });

        await batch.commit();

        res.status(200).send({ message: 'Samples added successfully!' });
    } catch (error) {
        console.error('Error adding samples:', error);
        res.status(500).send({ message: 'Error adding samples.', error: error.message });
    }
});


const storage = new Storage();
const bucket = admin.storage().bucket();

exports.exportAllToCSV = functions.https.onRequest((req, res) => {
    cors(req, res, async () => {
        try {
            const collections = await db.listCollections();
            const timestampCollections = collections.filter((col) =>
                col.id.startsWith("Sampling_")
            );

            if (timestampCollections.length === 0) {
                return res.status(404).send({ message: "No uploaded data found to export." });
            }

            for (const collection of timestampCollections) {
                const collectionName = collection.id;

                const snapshot = await db.collection(collectionName)
                    .orderBy("sample_index", "asc")
                    .get();

                if (snapshot.empty) {
                    console.log(`No data in collection ${collectionName}`);
                    continue;
                }

                const rows = [];
                rows.push(["TIMESTAMP", "SENSOR_ID", "SENSOR_MODEL", "UNIT_MAC", "SAMPLE", "SAMPLE_INDEX"]); // CSV headers
                snapshot.forEach(doc => {
                    const data = doc.data();
                    rows.push([data.timestamp, data.sensorID, data.sensorModel, data.unitMac, data.sample, data.sample_index]);
                });

                const csvString = stringify(rows, { header: false });
                const fileName = `${collectionName}.csv`;
                const file = bucket.file(fileName);

                await file.save(csvString);
                console.log(`Exported ${collectionName} to ${fileName}`);

                await deleteCollection(collectionName);
                console.log(`Deleted collection ${collectionName}`);
            }

            res.status(200).send({ message: "Exported data to Cloud Storage successfully." });
        } catch (error) {
            console.error("Error exporting collections:", error);
            res.status(500).send({ error: "An error occurred while exporting data." });
        }
    });
});
// Function to delete documents in a collection
async function deleteCollection(collectionPath) {
    const collectionRef = db.collection(collectionPath);
    const snapshot = await collectionRef.get();

    if (snapshot.empty) {
        console.log(`No documents found in collection ${collectionPath}`);
        return;
    }

    const batch = db.batch();
    snapshot.forEach(doc => {
        batch.delete(doc.ref);
    });

    await batch.commit();
    console.log(`All documents deleted from ${collectionPath}`);
}

app.get('/exportAll', exports.exportAllToCSV);

exports.api = functions.https.onRequest(app);
