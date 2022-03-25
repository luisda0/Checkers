//=================================
//======== Settings Modal =========
//=================================

window.addEventListener('load', event => {
    //close the modal
    document.getElementById('close-settings').addEventListener('click', event => {
        api.close_window('');
    });

    //game mode selectoin buttons
    let game_modes_btns = document.querySelectorAll('.settings-game-mode .button');
    for (i = 0; i < game_modes_btns.length; i++) {
        game_modes_btns[i].addEventListener('click', game_mode_select);
    }

    //piece color selection buttons
    let piece_btns = document.querySelectorAll('.settings-pieces-wrapper .button');
    for (i = 0; i < piece_btns.length; i++) {
        piece_btns[i].addEventListener('click', piece_select);
    }

    document.getElementById('start-game').addEventListener('click', start_game);
})



function game_mode_select() {
    let game_modes_btns = document.querySelectorAll('.settings-game-mode .button');
    let parent = document.getElementById('game-mode');

    if (this.classList.contains('option-selected')) {
        return;
    }

    if (parent.getAttribute('data-mode') == 'hc') {
        parent.dataset.mode = 'hh';
    }
    else {
        parent.dataset.mode = 'hc';
    }

    for (i = 0; i < game_modes_btns.length; i++) {
        game_modes_btns[i].classList.toggle('option-selected');
    }
}

function piece_select() {
    let piece_btns = document.querySelectorAll('.settings-pieces-wrapper .button');
    let parent = document.getElementById('piece-color');

    if (this.classList.contains('option-selected')) {
        return;
    }

    if (parent.getAttribute('data-color') == 'black') {
        parent.dataset.color = 'white';
    }
    else {
        parent.dataset.color = 'black';
    }

    for (i = 0; i < piece_btns.length; i++) {
        piece_btns[i].classList.toggle('option-selected');
    }
}

function start_game() {
    let mode = document.getElementById('game-mode').getAttribute('data-mode');
    let color = document.getElementById('piece-color').getAttribute('data-color');
    let difficulty = document.getElementById('difficulty-range').value;

    let args = [mode, color, difficulty];
    api.start_game(args);
}