export default class View {
    _errorMsg = "Something went wrong";
    _successMsg = "Your operation is successful! :)";
    _btnClose = document.querySelector(".close-modal");
    _modal = document.querySelector(".modal");
    _overlay = document.querySelector(".overlay");
    _parentEl;

    constructor() {
        this._addHandlerHideModal();
    }

    renderSpinner() {
        const html = `<div class="spinner">---</div> `;

        this._clear();
        this._parentEl.insertAdjacentHTML("afterbegin", html);
    }

    _toggleWindow() {
        this._overlay.classList.toggle("hidden");
        this._modal.classList.toggle("hidden");
    }

    _addHandlerHideModal() {
        this._btnClose.addEventListener("click", this._toggleWindow.bind(this));
        this._overlay.addEventListener("click", this._toggleWindow.bind(this));
    }

    renderError(msg = this._errorMsg) {
        const html = `
        <div class="modal">
		    <button class="close-modal">&times;</button>
		    <h1>OH! An error occured!</h1>
		    <p>
                ${msg}
		    </p>
	    </div>
        <div class="overlay"></div>`;

        this._clear();
        this._parentEl.insertAdjacentHTML("afterbegin", html);
    }

    renderSuccess(msg = this._successMsg) {
        const html = `
        <div class="modal">
		    <button class="close-modal">&times;</button>
		    <h1>Perfect! Everything is correct! :)</h1>
		    <p>
                ${msg}
		    </p>
	    </div>
        <div class="overlay"></div>`;

        this._clear();
        this._parentEl.insertAdjacentHTML("afterbegin", html);
    }

    _clear() {
        this._parentEl.innerHTML = "";
    }
};