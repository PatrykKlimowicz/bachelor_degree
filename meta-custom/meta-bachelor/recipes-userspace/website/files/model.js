import { SERVER_URL } from "./config.js";
import { AJAX } from "./helper.js";

export const state = {
    led: 0,
    randomSequence: [],
};

export const setLedMode = async function (newMode) {
    try {
        const data = await AJAX(`${SERVER_URL}led-mode/${newMode}`);
        state.led = data.mode;
    } catch (error) {
        throw error;
    }
};

export const getRandomSequence = async function (lenght) {
    try {
        const data = await AJAX(`${SERVER_URL}random-sequence/${lenght}`);
        state.randomSequence = data.random;
    } catch (error) {
        throw error;
    }
};
