//=================================
//======== Rules Modal =========
//=================================

window.addEventListener('load', event => {
    document.getElementById('close-rules').addEventListener('click', event => {
        api.close_window();
    });
});