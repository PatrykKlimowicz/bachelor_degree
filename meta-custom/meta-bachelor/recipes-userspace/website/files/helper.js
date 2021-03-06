import { TIMEOUT_SEC } from "./config.js";

const timeout = function (s) {
    return new Promise(function (_, reject) {
        setTimeout(function () {
            reject(new Error(`Request took too long! Timeout after ${s} second`));
        }, s * 1000);
    });
};

export const AJAX = async function (url, isPost = false) {
    try {
        const fetchPromise = isPost
            ? fetch(url, {
                  method: "POST",
                  mode: 'cors',
              })
            : fetch(url);

        // get data from API, set timeout for request
        const res = await Promise.race([fetchPromise, timeout(TIMEOUT_SEC)]);
        const data = await res.json();

        if (!res.ok) throw new Error(`${data.message}, ${res.status}`);

        return data;
    } catch (error) {
        // rethrow the error
        throw error;
    }
};
