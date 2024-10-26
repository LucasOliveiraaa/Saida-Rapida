if (!location.href.includes("feature"))
    return;

const { contextBridge } = require("electron")

const { inflate } = require("pako")
const zmq = require("zeromq")

function decompress(data) {
    data = new Uint8Array(data)
    data = inflate(data, { to: "string" })
    return data
}

async function receive(callback, camera, reconeye, basededados, fps) {
    const sock = new zmq.Subscriber()
    sock.connect("tcp://127.0.0.1:5555")
    sock.subscribe("")

    let date = Date.now()
    let frames = 0
    for await (const [buffer] of sock) {
        frames++
        const { descriptions, frame } = JSON.parse(decompress(buffer))


        const now = Date.now()
        const elapsed = now - date
        date = now
        let FPS = Math.round(frames / (elapsed / 1000))
        fps.innerText = FPS
        if (FPS > 50) {
            fps.className = "status-success"
        } else if (FPS > 20) {
            fps.className = ""
        } else {
            fps.className = "status-warn"
        }
        if (elapsed >= 1000) {
            frames = 0
        }

        reconeye.innerText = "Conectado"
        reconeye.className = "status-success"

        basededados.innerText = "Conectado"
        basededados.className = "status-success"

        camera.src = `data:image/png;base64,${frame}`

        for (const { main, id, data } of descriptions) {
            await callback(main, id, data)
        }
    }
}

const admin = require("firebase-admin/app")
const firestore = require("firebase-admin/firestore")

const credentials = require("/home/lucas/key.json");

admin.initializeApp({
    credential: admin.cert(credentials)
});

const db = firestore.getFirestore()
const school = db.collection("escolas/LFplnjWNSN0amNh0Da79/historico")
const students = db.collection("escolas/LFplnjWNSN0amNh0Da79/alunos")

const facial_records = db.collection("/escolas/LFplnjWNSN0amNh0Da79/facial")

contextBridge.exposeInMainWorld("glob", {
    receive,
    async getInside(id) {
        const ref = school.doc("srgate-" + Math.floor(Math.random() * 100000))
        await ref.set({
            aluno: id,
            hora: new Date(),
            autorizadoPor: "Sr. Gate",
            status: "naSala"
        })
    },
    async getStudent(id) {
        return (await students.doc(id).get()).data()
    },
    async saveNewDescriptor(id, data) {
        const ref = facial_records.doc(id)
        await ref.set({
            data
        })
    }
})