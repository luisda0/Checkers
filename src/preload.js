const {ipcRenderer, contextBridge} = require("electron");

contextBridge.exposeInMainWorld('api', {
    isFullScreen: () => ipcRenderer.invoke("is-fullscreen"), 
    exitFullScreen: () => ipcRenderer.invoke("exit-fullscreen"),
    initialize_game: () => ipcRenderer.invoke('initialize_game'),
    clear_history: () => ipcRenderer.invoke('clear_history'),
    make_play: (args) => ipcRenderer.invoke('make_play', args),
    button_coordinates: () => ipcRenderer.invoke('button_coordinates'),
    computer_play: () => ipcRenderer.invoke('computer_play'),
    get_board: () => ipcRenderer.invoke('get_board'),
    show_settings: () => ipcRenderer.invoke('show_settings'),
    close_window: (arg) => ipcRenderer.invoke('close_window', arg),
    set_depth: (arg) => ipcRenderer.invoke('set_depth', arg),
    set_turn: (arg) => ipcRenderer.invoke('set_turn', arg),
    piece_img: (arg) => ipcRenderer.invoke('piece_img', arg),
    start_game: (args) => ipcRenderer.invoke('start_game', args),
    show_winner: (arg) => ipcRenderer.invoke('show_winner', arg),
    show_restart: () => ipcRenderer.invoke('show_restart'),
    confirm_new_game: () => ipcRenderer.invoke('confirm_new_game'),
    receive: (channel, func) => {
        let validChannels = [
            "go_forward",
            "go_back", 
            "new_game", 
            "confirm_restart",
            "check_open_settings"
        ];
        if (validChannels.includes(channel)) {
            ipcRenderer.on(channel, (event, args) => func(args));
        }
    }
});