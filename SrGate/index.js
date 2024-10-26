const { app, BrowserWindow } = require("electron")
const path = require("path")

function createWindow() {
    const win = new BrowserWindow({
        autoHideMenuBar: true,
        webPreferences: {
            preload: path.join(__dirname, "src/assets/js/preload.js"),
            nodeIntegration: true,
            contextIsolation: true
        }
    })

    win.webContents.openDevTools()

    win.loadFile(path.join(__dirname, "src/index.html"))
}

app.whenReady()
    .then(() => {
        createWindow()
    })