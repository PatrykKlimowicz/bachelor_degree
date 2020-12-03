class RandomView {
    _errorMsg = "Something went wrong";
    _parentEl = document.querySelector(".random-sequence");
    _randomSequenceField = document.querySelector(".random-sequence--output");

    addHandlerRandom(handler) {
        this._parentEl.addEventListener("click", function (e) {
            e.preventDefault();

            const btn = e.target.closest(".random-sequence--submit");
            if (!btn) return;

            const length = +document.querySelector(".random-sequence--input").value;
            if (!length && length !== 0) return;

            handler(length);
        });
    }

    renderError(msg = this._errorMsg) {
        this._randomSequenceField.textContent = msg;
    }

    renderRandomSequence(data) {
        this._randomSequenceField.textContent = data;
    }
}

export default new RandomView();
