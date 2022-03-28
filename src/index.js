const { rejects } = require('assert');
const { info } = require('console');
const { dialog, app, BrowserWindow, ipcMain, Menu, shell, webContents } = require('electron');
const path = require('path');
const checkers = require('../build/Debug/checkers');
const config = require('../checkers-config');

var rulesModal;
var settingsModal;
var mainWindow;

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) {
  // eslint-disable-line global-require
  app.quit();
}

const createWindow = () => {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 1000,
    height: 800,
    minHeight: 640,
    minWidth: 830,
    webPreferences: {
      //devTools: false,
      preload: path.join(app.getAppPath(), 'src/preload.js'),
      sandbox: true,
      nodeIntegration: false,
      contextIsolation: true,
      enableRemoteModule: false
    }
  });
  // start with a mazimized window
  mainWindow.maximize();

  // and load the index.html of the app.
  mainWindow.loadFile(path.join(__dirname, 'index.html'));

  // build main menu
  const isMac = process.platform === 'darwin';
  const template = [
    // { role: 'appMenu' }
    ...(isMac ? [{
      label: 'Checkers',
      submenu: [
        { role: 'about' },
        { type: 'separator' },
        { role: 'services' },
        { type: 'separator' },
        { role: 'hide' },
        { role: 'hideOthers' },
        { role: 'unhide' },
        { type: 'separator' },
        { role: 'quit' }
      ]
    }] : []),
    // { role: 'fileMenu' }
    {
      label: 'Game',
      submenu: [
        { 
          label: 'New Game',
          accelerator: 'CmdOrCtrl+N',
          click: () => {
            mainWindow.webContents.send('check_open_settings');
          }
        },
        {
          label: 'Restart...',
          click: () => {
            mainWindow.webContents.send('confirm_restart');
          }
        },
        isMac ? { role: 'close' } : { role: 'quit' }
      ]
    },
    // { role: 'editMenu' }
    {
      label: 'Edit',
      submenu: [
        { 
          label: 'Undo Move',
          accelerator: 'CmdOrCtrl+Z',
          click: () => { 
            try {
              mainWindow.webContents.send('go_back', checkers.go_back());
            } catch (error) {
              mainWindow.webContents.send('go_back', error.message);
            }
          }
        },
        { 
          label: 'Redo Move',
          accelerator: 'Shift+CmdOrCtrl+Z',
          click: () => { 
            try {
              mainWindow.webContents.send('go_forward', checkers.go_forward());
            } catch (error) {
              mainWindow.webContents.send('go_forward', error.message);
            }
          }
        },
        { type: 'separator' },
        { role: 'cut' },
        { role: 'copy' },
        { role: 'paste' },
        ...(isMac ? [
          { role: 'pasteAndMatchStyle' },
          { role: 'delete' },
          { role: 'selectAll' },
          { type: 'separator' },
          {
            label: 'Speech',
            submenu: [
              { role: 'startSpeaking' },
              { role: 'stopSpeaking' }
            ]
          }
        ] : [
          { role: 'delete' },
          { type: 'separator' },
          { role: 'selectAll' }
        ])
      ]
    },
    // { role: 'viewMenu' }
    {
      label: 'View',
      submenu: [
        { role: 'toggleDevTools' },
        { type: 'separator' },
        { role: 'resetZoom' },
        { role: 'zoomIn' },
        { role: 'zoomOut' },
        { type: 'separator' },
        { role: 'togglefullscreen' }
      ]
    },
    ...(isMac ? [{ role: 'windowMenu' }] : []),
    {
      role: 'help',
      submenu: [
        {
          label: 'Game Rules',
          accelerator: 'CmdOrCtrl+R',
          click: () => { show_rules(); }
        },
        {
          label: 'Learn More',
          click: async () => {
            const { shell } = require('electron')
            await shell.openExternal('https://github.com/luisda0/Checkers')
          }
        },
        {
          label: 'Report a Problem',
          click: async () => {
            const { shell } = require('electron')
            await shell.openExternal('https://github.com/luisda0/Checkers/issues')
          }
        }
      ]
    }
  ];  
  const mainMenu = Menu.buildFromTemplate(template);
  Menu.setApplicationMenu(mainMenu);

  // Open the DevTools.
  mainWindow.webContents.openDevTools();
};

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

//Modal windows
const createSettingsModal = () => {
  // Create the browser window.
  settingsModal = new BrowserWindow({
    parent: mainWindow,
    modal: true,
    show: false,
    width: 400,
    height: 475,
    hasShadow: true,
    resizable: false,
    minimizable: false,
    maximizable: false,
    movable: false,
    frame: false,
    alwaysOnTop: true,
    webPreferences: {
      devTools: false,
      preload: path.join(app.getAppPath(), 'src/preload.js'),
      sandbox: true,
      nodeIntegration: false,
      contextIsolation: true,
      enableRemoteModule: false
    }
  });

  // and load the index.html of the modal.
  settingsModal.loadFile(path.join(__dirname, 'modal.html'));

  settingsModal.on('close', () => {
    settingsModal = null;
  });

  //show settings modal
  settingsModal.once('ready-to-show', () => {
    settingsModal.show();
  });

};

const createRulesModal = () => {
  rulesModal = new BrowserWindow({
    parent: mainWindow,
    modal: true,
    show: false,
    width: 600,
    height: 600,
    hasShadow: true,
    minimizable: false,
    maximizable: false,
    resizable: true,
    movable: true,
    frame: true,
    closable: true,
    webPreferences: {
      //devTools: false,
      preload: path.join(app.getAppPath(), 'src/preload.js'),
      sandbox: true,
      nodeIntegration: false,
      contextIsolation: true,
      enableRemoteModule: false
    }
  });

  // and load the index html of the modal
  rulesModal.loadFile(path.join(__dirname, 'rules.html'));

  rulesModal.on('close', () => {
    rulesModal = null;
  })

  // show rules modal
  rulesModal.once('ready-to-show', () => {
    rulesModal.show();
  });
};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow);
app.on('ready', createSettingsModal);

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and import them here.

function show_settings() {
  if (settingsModal == null) {
    createSettingsModal();
  } 
}

function show_rules() {
  if (rulesModal == null) {
    createRulesModal();
  }
}

//This section handles all the requests made form renderer

//to exit full screen mode by pressing "esc" key
ipcMain.handle("is-fullscreen", () => {
  let fullscreen = BrowserWindow.getFocusedWindow()?.isMaximized();
  return fullscreen;
});

ipcMain.handle("exit-fullscreen", () => {
  return BrowserWindow.getFocusedWindow()?.setFullScreen(false);
});

//to initialize the game
ipcMain.handle("initialize_game", () => {
  return checkers.initialize_game();
});

ipcMain.handle("start_game", (event, args) => {
  BrowserWindow.getFocusedWindow().close();
  mainWindow.webContents.send('new_game', args);
});

ipcMain.handle("clear_history", () => {
  return checkers.clear_history();
});

ipcMain.handle("make_play", (event, args) => {
  try {
    return checkers.make_play(args[0], args[1], args[2], args[3]);
  }
  catch(error) {
    throw error.message;
  }
});

ipcMain.handle("button_coordinates", () => {
  try {
    return config.button_coordinates;
  }
  catch(error) {
    return error;
  }
});

ipcMain.handle("computer_play", () => {
  try {
    return checkers.computer_play();
  }
  catch(error) {
    return error.message;
  }
});

ipcMain.handle("get_board", () => {
  try {
    return checkers.get_board();
  }
  catch(error) {
    return error.message;
  }
});

ipcMain.handle("show_settings", () => {
    show_settings()
});

ipcMain.handle("close_window", () => {
  BrowserWindow.getFocusedWindow().close();
});

ipcMain.handle("set_depth", (event, arg) => {
  try {
    let depth = config.difficulty_2_depth[`${arg}`];
    checkers.set_depth(depth);
  }
  catch {
    return 'Depth was not set';
  }
});

ipcMain.handle("set_turn", (event, arg) => {
  try {
    checkers.set_turn(arg);
  }
  catch {
    return error.message;
  }
});

ipcMain.handle("piece_img", (event, arg) => {

  return config.piece_img[arg];

});

ipcMain.handle("show_winner", (event, arg) => {
  let options = {
    message: arg,
    type: 'none',
    buttons: ['Play Again', 'New Game'],
    title: 'Game Over',
  };

  return dialog.showMessageBox(mainWindow, options);
});

ipcMain.handle("show_restart", (event, arg) => {
  let options = {
    message: "Are you sure you want to restart this game?",
    type: "question",
    buttons: ['Cancel', 'OK'],
    defaultId: 0,
    detail: "Your current game will be discarded.",
    cancelId: 0,
    title: 'Restart Game',
  };

  return dialog.showMessageBox(mainWindow, options);
});

ipcMain.handle("confirm_new_game", () => {
  let options = {
    message: "Are you sure you want to start a new game?",
    type: "question",
    buttons: ['Cancel', 'OK'],
    defaultId: 0,
    detail: "Your current game will be discarded.",
    cancelId: 0,
    title: 'New Game',
  };

  return dialog.showMessageBox(mainWindow, options);
});