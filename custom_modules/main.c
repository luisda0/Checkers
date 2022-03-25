#include <node_api.h>
#include "definitions.h"

napi_value MyFunction(napi_env env, napi_callback_info info) {
    napi_status status;

    //get values passed into the function
    size_t argc = 1; //number of expected arguments
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }

    //convert arguments to C datatypes
    int number = 0;
    status = napi_get_value_int32(env, argv[0], &number);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }

    //internal C logic
    number = number + 2;

    //convert C datatypes to JS datatypes and return them
    napi_value myNumber;
    status = napi_create_int32(env, number, &myNumber);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    return myNumber;
}

napi_value initializeGame(napi_env env, napi_callback_info info) {
    napi_status status;

    initializeHistory();
    char *boardCharArray = returnBoard();

    napi_value output;
    status = napi_create_string_utf8(env, boardCharArray, 34, &output);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    free(boardCharArray);
    return output;
}

napi_value eraseHistory(napi_env env, napi_callback_info info) {

    clearHistory();
    
    return NULL;
}

napi_value makeAPlay(napi_env env, napi_callback_info info) {
    //get values passed into the function
    napi_status status;
    size_t argc = 4; //number of expected arguments
    napi_value argv[4];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }

    //get the values of the coordinates of the play made
    int initial[2], final[2];
    status = napi_get_value_int32(env, argv[0], &initial[0]);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }
    status = napi_get_value_int32(env, argv[1], &initial[1]);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }
    status = napi_get_value_int32(env, argv[2], &final[0]);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }
    status = napi_get_value_int32(env, argv[3], &final[1]);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }

    int arrayLength;
    char *boardCharArray = NULL;
   
    //check if there is a path for the selected move
    int result = playerMove(initial, final);
    if (result == -1) {
        napi_throw_error(env, NULL, "Incorrect move");
    }
    //return the player who won as a string
    if (result != 0) {
        boardCharArray = malloc(2 * sizeof(char *));
        sprintf(boardCharArray, "%d", result);
        arrayLength = 2;
    }
    //if no one won return the board
    else {
        boardCharArray = returnBoard();
        arrayLength = 34;
    }

    napi_value output;
    status = napi_create_string_utf8(env, boardCharArray, arrayLength, &output);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    free(boardCharArray);
    return output;
}

napi_value goBack(napi_env env, napi_callback_info info) {
    napi_status status;
    
    if (!goBackInHistory()){
        napi_throw_error(env, NULL, "Cannot go back");
    }
    char *boardCharArray = returnBoard();

    napi_value output;
    status = napi_create_string_utf8(env, boardCharArray, 34, &output);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    free(boardCharArray);
    return output;
}

napi_value goForward(napi_env env, napi_callback_info info) {
    napi_status status;

    if (!goForwardInHistory()){
        napi_throw_error(env, NULL, "Cannot go forward");
    }
    char *boardCharArray = returnBoard();

    napi_value output;
    status = napi_create_string_utf8(env, boardCharArray, 34, &output);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    free(boardCharArray);
    return output;
}

napi_value computerPlay(napi_env env, napi_callback_info info) {
    napi_status status;

    int arrayLength;
    char *boardCharArray = NULL;
   
    //execute the computer's next move
    int result = computerMove();

    //return the player who won as a string
    if (result != 0) {
        boardCharArray = malloc(2 * sizeof(char *));
        sprintf(boardCharArray, "%d", result);
        arrayLength = 2;
    }
    //if no one won return the board
    else {
        boardCharArray = returnBoard();
        arrayLength = 34;
    }

    napi_value output;
    status = napi_create_string_utf8(env, boardCharArray, arrayLength, &output);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    free(boardCharArray);
    return output;
}

napi_value getBoard(napi_env env, napi_callback_info info) {
    napi_status status;

    char *boardCharArray = returnBoard();

    napi_value output;
    status = napi_create_string_utf8(env, boardCharArray, 34, &output);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }
    free(boardCharArray);
    return output;
}

napi_value setTurn(napi_env env, napi_callback_info info) {
    napi_status status;

    //get values passed into the function
    size_t argc = 1; //number of expected arguments
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }

    //convert arguments to C datatypes
    int _turn = 2;
    status = napi_get_value_int32(env, argv[0], &_turn);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }

    if (_turn != 1 && _turn != 2) {
        napi_throw_error(env, NULL, "Invalid number was passed");
    }
    _setTurn(_turn);

    return NULL;
}

napi_value setMiniMaxDepth(napi_env env, napi_callback_info info) {
    napi_status status;

    //get values passed into the function
    size_t argc = 1; //number of expected arguments
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }

    //convert arguments to C datatypes
    int _depth = 2;
    status = napi_get_value_int32(env, argv[0], &_depth);
    if (status != napi_ok) {
        napi_throw_type_error(env, NULL, "Invalid number was passed as argument");
    }

    _setMiniMaxDepth(_depth);

    return NULL;
}

//####################### Exports ########################
napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fn;

    status = napi_create_function(env, NULL, 0, MyFunction, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "my_function", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, initializeGame, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "initialize_game", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, makeAPlay, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "make_play", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, goBack, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "go_back", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, goForward, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "go_forward", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, computerPlay, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "computer_play", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, getBoard, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "get_board", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, setTurn, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "set_turn", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, setMiniMaxDepth, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "set_depth", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    status = napi_create_function(env, NULL, 0, eraseHistory, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "unable to wrap native function");
    }
    status = napi_set_named_property(env, exports, "clear_history", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to pupulate exports");
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)