class LEDView {
    _form = document.querySelector(".led-state-form");
    _infoArea = document.querySelector(".led-info");
    _errorMsg = "Something went wrong";

    _getRadiodata() {
        console.log(this._form);
        const data = new FormData(this._form);
        console.log(data);
        return data;
    }

    addHandlerMode(handler) {
        this._form.addEventListener("submit", function (e) {
            e.preventDefault();
            const dataArr = [...new FormData(this)];
            const data = Object.fromEntries(dataArr);

            if (!+data.ledMode) return;

            handler(+data.ledMode);
        });
    }

    renderMessage(msg = this._errorMsg) {
        this._infoArea.textContent = msg;
    }
}

export default new LEDView();
