const game = {
    game_in_progress: false,
    turn: 2,
    pressed_buttons: [ null, null ],
    mode: 'hc',
    difficulty: 1,
    players: [null, null],
    pieces: {
        man_team1: '',
        king_team1: '',
        man_team2: '',
        king_team2: ''
    } ,
    setMode: (mode) => {
        //hh = Human vs. Human
        //hc = Human vs. Computer
        if (mode != 'hh' && mode != 'hc') {
            return false;
        }

        game.mode = mode;

        return true;
    } ,
    setColor: (color) => {
        return new Promise((resolve, reject) => {
            if (color != 'black' && color != 'white') {
                reject(
                    Error('Invalid value')
                );
            }
    
            if (color == 'black') {
                Promise.allSettled([
                    api.piece_img('man_white'),
                    api.piece_img('king_white'),
                    api.piece_img('man_black'),
                    api.piece_img('king_black'),
                    api.set_turn(2)
                ])
                .then(values => {
                    game.pieces.man_team1 = `<img data-team="1" src="${values[0].value}" />`;
                    game.pieces.king_team1 = `<img data-team="1" src="${values[1].value}" />`;
                    game.pieces.man_team2 = `<img data-team="2" src="${values[2].value}" />`;
                    game.pieces.king_team2 = `<img data-team="2" src="${values[3].value}" />`;
                    game.turn = 2;
                    game.players[0] = 'white';
                    game.players[1] = 'black';
                    resolve(true);
                })
                .catch(error => {
                    reject(Error(error.message));
                })
            }
            else {
                Promise.allSettled([
                    api.piece_img('man_black'),
                    api.piece_img('king_black'),
                    api.piece_img('man_white'),
                    api.piece_img('king_white'),
                    api.set_turn(1)
                ])
                .then(values => {
                    game.pieces.man_team1 = `<img data-team="1" src="${values[0].value}" />`;
                    game.pieces.king_team1 = `<img data-team="1" src="${values[1].value}" />`;
                    game.pieces.man_team2 = `<img data-team="2" src="${values[2].value}" />`;
                    game.pieces.king_team2 = `<img data-team="2" src="${values[3].value}" />`;
                    game.turn = 1;
                    game.players[0] = 'black';
                    game.players[1] = 'white';
                    resolve(true);
                })
                .catch(error => {
                    reject(Error(error.message));
                })
            }
        });
    } ,
    setDifficulty: (diff) => {
        return new Promise((resolve, reject) => {
            if (diff < 1 || diff > 5) {
                reject(
                    Error('Invalid value')
                );
              }
              api.set_depth(diff)
              .then(() => {
                game.difficulty = diff;
                resolve(true);
              })
              .catch((error) => {
                  reject(
                      Error(error.message)
                  );
              });
        });
    } ,
    newGame: (mode, color, diff) => {
        return new Promise((resolve, reject) => {
            if (game.game_in_progress) {
                reject(
                    Error('Game in progress')
                );
            }


            game.setColor(color)
            .then((result) => {
                return game.setDifficulty(diff);
            })
            .catch((error) => {
                reject(
                    Error(error.message)
                );
            })
            .then(result => {
                //initialize the game and display the board to the player
                return api.initialize_game()
            })
            .catch(error => {
                reject(
                    Error(error.message)
                );
            })
            .then((result) => {
                if (game.setMode(mode)) {
                    display_pieces(result);
                    game.game_in_progress = true;
                    resolve(true);
                }
                reject(
                    Error('Invalid game mode')
                );
            })
            .catch((error) => {
                reject(
                    Error(error.message)
                );
            });
        });
    }
}

document.addEventListener("keydown", event => {
    switch (event.key) {
        case "Escape":
            if (api.isFullScreen()) {
                api.exitFullScreen()
            }
            break;
         }
});
/*
window.addEventListener("resize", event => {
    let board = document.getElementsByClassName('board')[0];
    let height = window.innerHeight;
    let width = window.innerWidth;
    let boardHeight = board.offsetHeight;
    let boardWidht = board.offsetWidth;
});
*/
window.addEventListener("load", event => {
    activate_buttons();
});
  
api.receive("go_back", (result) => {
    try {
        if (result[0] != '[') {
            throw result;
        }
        display_pieces(result);
        game.turn = (game.turn == 2)? 1 : 2;
    }
    catch(error) {
        console.log(error);
    }
});

api.receive("go_forward", (result) => {
    try {
        if (result[0] != '[') {
            throw result;
        }
        display_pieces(result);
        game.turn = (game.turn == 2)? 1 : 2;
    }
    catch(error) {
        console.log(error);
    }
});




//gets the input from the user
//input: button pressed by the user
//checks the button pressed and makes the play if valid
function button_was_pressed() {
    let buttonId = this.getAttribute('data-id');
    let opponent = (game.turn == 2)? 1 : 2;
    
    //remove any buttons pressed from the last turn
    remove_last_selections();

    //if an opponents piece is selected
    if (this.innerHTML != '') {
        if (this.firstChild.getAttribute('data-team') == `${opponent}`) {
            //exit, it's the opponents piece
            display_wrong_choice.apply(this);
            return;
        }
    }
    //check initial position
    if (game.pressed_buttons[0] == null) {
        //check if the button pressed is from the player's team
        if (!this.hasChildNodes()) {
            //exit, it is a blank space
            display_wrong_choice.apply(this);
            return;
        }
        if (this.firstChild.getAttribute('data-team') != `${game.turn}`) {
            //exit, it's the opponents piece
            display_wrong_choice.apply(this);
            return;
        }
        display_current_choice.apply(this);
        game.pressed_buttons[0] = buttonId;
    }
    else {
        //if they press the same button twice
        if (game.pressed_buttons[0] == buttonId) {
            //de-select the current choice
            game.pressed_buttons[0] = null;
            display_current_choice.apply(this);
            return;
        }
        if (this.innerHTML != '') {
            //if the player selects another one of his pieces
            if (this.firstChild.getAttribute('data-team') == `${game.turn}`) {
                //deselect last choice
                let pastSelection = document.querySelectorAll(`button[data-id="${game.pressed_buttons[0]}"]`)[0];
                display_current_choice.apply(pastSelection);
                //select new choice
                game.pressed_buttons[0] = buttonId;
                display_current_choice.apply(this);
                return;
            }
        }
        else {
            game.pressed_buttons[1] = buttonId;
            execute_play()
            .then(() => {
                next_play();
            })
            .catch(error => {
                console.log(error.message);
            });
        }
    }
}

//Makes the call to the api to execute the play, made by the human
function execute_play() {
    return new Promise((resolve, reject) => {
        //get the coordinates of the pressed buttons
        id_to_coordinate().then((coordinates) => {
            //execute the play
            api.make_play(coordinates)
            .then((result) => {
                if (result.length > 3) {
                    display_pieces(result);
                    let lastSelection = document.querySelectorAll(`button[data-id="${game.pressed_buttons[1]}"]`)[0];
                    display_current_choice.apply(lastSelection);
                    if (game.mode == 'hc') {
                        game.pressed_buttons[0] = null;
                        game.pressed_buttons[1] = null;
                    }
                    game.turn = (game.turn == 2)? 1 : 2;
                    resolve();
                }
                else {
                    display_win(result[0]);
                    resolve();
                }
            })
            .catch((error) => {
                //display error to user
                let lastSelection = document.querySelectorAll(`button[data-id="${game.pressed_buttons[1]}"]`)[0];
                display_wrong_choice.apply(lastSelection);
                //extract error message
                errorMessage = error.message.split(':')[1].slice(1);
                reject( Error(errorMessage) );
            });
        })
        .catch((error) => {
            console.log(error);
        });
    });
}

//Checks whether the computer has to move next
function next_play() {
    if (game.mode == 'hh') {
        return;
    }

    computer_think();
}

//Tells the computer to make a play
function computer_think() {
    deactivate_buttons();

    const minWaitTime = new Promise((resolve, reject) => {
        setTimeout(resolve, 1000);
    });

    Promise.all([
        api.computer_play(),
        minWaitTime
    ])
    .then((result) => {
        remove_all_selections();
        if (result[0].length > 3) {
            display_pieces(result[0]);
            game.turn = (game.turn == 2)? 1 : 2;
            activate_buttons();
        }
        else {
            activate_buttons();
            display_win(result[0]);
        }
    })
    .catch((error) => {
        console.log(error);
    });
}

//translates a button id into a set of x,y coordinates
//in: array of buttons pressed
//out: promise with array of coordinates
function id_to_coordinate() {
    return new Promise((resolve, reject) => {
        api.button_coordinates()
        .then((buttonCoordinates) => {
            let coordinates = Array(4);
            let button1 = parseInt(game.pressed_buttons[0]);
            let button2 = parseInt(game.pressed_buttons[1]);
            coordinates[0] = buttonCoordinates[button1][0];
            coordinates[1] = buttonCoordinates[button1][1];
            coordinates[2] = buttonCoordinates[button2][0];
            coordinates[3] = buttonCoordinates[button2][1];
            if (!coordinates.includes(undefined)) {
                resolve(coordinates);
            }
            reject('Empty coordinates');
        })
        .catch((error => {
            console.log(error);
        }));
    }); 
}

//sets a shadow arround the selected quadrant
//removes the shadow if one is already set (if user de-selects)
function display_current_choice() {
    if (this.style.boxShadow == '') {
        this.style.boxShadow = "0 0 13px 3px #77d377 inset";
        this.style.border = "unset";
    }
    else {
        this.style.boxShadow = null;
    }
}

//sets a red background momentarily to show the user
//that they can't move a piece
function display_wrong_choice() {
    this.style.backgroundColor = "#932121";
    let timeout = setTimeout(() => {
        this.style.backgroundColor = null;
        clearTimeout(timeout);
    }, 500);
}

//diplay the pieces on the board
//input: string with the pieces present on the board
//out: changes are displayed to the user
function display_pieces(boardString) {
    let boardPieces = transform_board_string(boardString);

    for (i = 0; i < boardPieces.length; i++) {
        let square = boardPieces[i];
        let target = document.querySelectorAll(`button[data-id="${i}"]`)[0];

        if (square == 1) {
            target.innerHTML = game.pieces.man_team1;
        }
        else if (square == 2) {
            target.innerHTML = game.pieces.man_team2;
        } 
        else if (square == 3) {
            target.innerHTML = game.pieces.king_team1;
        }
        else if (square == 4) {
            target.innerHTML = game.pieces.king_team2;
        }
        else {
            target.innerHTML = '';
        }
    }    
}

//transform a string into a an int array
function transform_board_string(boardString) {
    boardString = boardString.slice(1,-1).split('');
    let boardPieces = boardString.map((i) => {
        return parseInt(i)
    });
    return boardPieces;
}

function display_win(winner) {
    alert('good');
    //convert to int
    winner = parseInt(winner);
    //game now not in progress
    game.game_in_progress = false;
    alert("it's the board");
    //display last move to user
    api.get_board().
    then((board) => {
        alert('good good');
        display_pieces(board);
        let message = '';

        if (game.mode == 'hc') {
            if (winner == 1) {
                message = "I beat you, better luck next time.";
            }
            if (winner == 2) {
                message = "You won! Congrats.";
            }
            if (winner == 3) {
                message = "It's a draw. Well played.";
            }
        }
        else {
            if (winner < 3) {
                message = `Congratulations, ${game.players[winner - 1]} pieces won!`;
            }
            else {
                message = "It's a draw. Who ever wins a match of indian leg wrestling wins this game.";
            }
        }
        alert(message);
        //display a pop up showing who won
        return api.show_winner(message);
    })
    .catch((error) => {
         console.log(error);
     })
     .then(result => {
         alert('not here');
         //if user selects to start a new game
        if (result.response == 1) {
            api.clear_history()
            .then(result => {
                settings();
            });
        }
        else {
            //if user selcts to restart this game
            api.clear_history()
            .then((result) => {
                if (game.players[0] == 'black') {
                    game.turn = 1;
                }
                else {
                    game.turn = 2;
                }
                return api.set_turn(game.turn);
            })
            .then(result => {
                game.pressed_buttons[0] = null;
                game.pressed_buttons[1] = null;
                return api.initialize_game();
            })
            .then(result => {
                display_pieces(result);
                game.game_in_progress = true;

                //if computer starts moving
                if (game.mode == 'hc' && game.players[0] == 'black') {
                    computer_think();
                }
            });
        }
     });
}

//Removes highlight of any buttons pressed in the last turn
function remove_last_selections() {
    let last_turn = (game.turn == 1)? 2: 1;
    let opponents = document.querySelectorAll(`img[data-team="${last_turn}"]`);
    //go through each of the buttons where the opponent is an see if it's selected
    for (i = 0; i < opponents.length; i++) {
        let button = opponents[i].parentElement;
        //de-select the button if selected
        if (button.style.boxShadow) {
            display_current_choice.apply(button);
        }
    }
}

//Removes highlight of all buttons pressed
function remove_all_selections() {
    const buttons = document.querySelectorAll("button.is-black");

    //go through each of the buttons and see if it's selected
    for (i = 0; i < buttons.length; i++) {
        //de-select the button if selected
        if (buttons[i].style.boxShadow) {
            display_current_choice.apply(buttons[i]);
        }
    }
}

//adds a click listener to all the black buttons in the board
function activate_buttons() {
    const buttons = document.querySelectorAll("button.is-black");

    //initialize buttons
    for (i = 0; i < buttons.length; i++) {
        buttons[i].addEventListener("click", button_was_pressed);
    }
}

//removes the event listener from all black buttons in the board
//buttons become inactive
function deactivate_buttons() {
    const buttons = document.querySelectorAll("button.is-black");

    //initialize buttons
    for (i = 0; i < buttons.length; i++) {
        buttons[i].removeEventListener("click", button_was_pressed);
    }
}

//open settings modal
function settings() {
    api.show_settings()
}

api.receive("new_game", (args) => {

    game.newGame(args[0], args[1], args[2])
    .then(result => {
        if (game.turn == 1 && game.mode == 'hc') {
            computer_think();
        }
    })

});