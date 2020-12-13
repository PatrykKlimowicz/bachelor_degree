import { SERVER_URL } from "./config.js";
import { AJAX } from "./helper.js";

export const state = {
    led: 0,
    randomSequence: [],
};

export const setLedMode = async function (newMode) {
    try {
        const data = await AJAX(`${SERVER_URL}led-mode/${newMode}`, true);
        console.log(data);
        state.led = +data.ledMode;
    } catch (error) {
        throw error;
    }
};

export const getRandomSequence = async function (lenght) {
    try {
        const data = await AJAX(`${SERVER_URL}random/${lenght}`);
        console.log(data);
        state.randomSequence = data.random;
    } catch (error) {
        throw error;
    }
};
