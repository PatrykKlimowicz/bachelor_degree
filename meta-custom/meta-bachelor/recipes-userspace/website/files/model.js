import { SERVER_URL } from "./config.js"
import { getJSON } from "./helpers.js";

export const state = {
    led: 0,
    randomSequence: [],
};

export const setLedMode = async function (newMode) {
    try {
        // code for setting led mode 
        throw(new Error("this is my error message"));
    } catch (error) {
        throw(new Error("this is my error message"));
    }
};

export const getRandomSequence = async function (lenght) {
    try {
        // get random sequence
        throw(new Error("this is my error message"));
    } catch (error) {
        throw(new Error("this is my error message"));
        
    }
};
