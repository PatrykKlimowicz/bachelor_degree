import * as model from "./model.js"

import ledView from "./LEDView.js"
import randomView from "./RandomView.js"

const controlLED = async function (mode) {
    try {
        // 1. Check if mode is in valid range
        if ( mode < 0 || mode > 3) return ledView.renderError(`Please select radio button`);
        
        // 2. Check if current mode is same as wanted
        if (mode === model.state.led) return ledView.renderSuccess(`Your LED is already in MODE ${mode} ;)`);

        // 3. Render loading spinner
        ledView.renderSpinner()

        // 4. Set new mode
        await model.setLedMode(mode);

        // 5. Render success message
        ledView.renderSuccess(`Your LED is now in MODE ${mode} :)`);
    } catch (error) {
        // Render error message
        ledView.renderError();
    }
};

const controlRandom = async function (lenght) {
    try {
        // 1. Check if length is in valid range
        if (lenght < 1 || lenght > 120) return randomView.renderError("Please input number between 1 and 120");
    
        // 2. Render loading spinner
        randomView.renderSpinner()

        // 3. Ask for random sequence
        await model.getRandomSequence(lenght);

        // 4. Render success message
        randomView.renderSuccess();

        // 5. Display random sequence
        randomView.renderRandomSequence(model.state.randomSequence);
    } catch (error) {
        // Render error message
        randomView.renderError();
    }
};

const init = function () {
    ledView.addHandlerMode(controlLED);
    randomView.addHandlerRandom(controlRandom);
};
init();
