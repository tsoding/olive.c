function make_environment(...envs) {
    return new Proxy(envs, {
        get(target, prop, receiver) {
            for (let env of envs) {
                if (env.hasOwnProperty(prop)) {
                    return env[prop];
                }
            }
            return (...args) => {console.error("NOT IMPLEMENTED: "+prop, args)}
        }
    });
}

const libm = {
    "atan2f": Math.atan2,
    "cosf": Math.cos,
    "sinf": Math.sin,
    "sqrtf": Math.sqrt,
};

async function startDemo(elementId, wasmPath) {
    const app = document.getElementById(elementId);
    if (app === null) {
        console.error(`Could not find element ${elementId}. Skipping demo ${wasmPath}...`);
        return;
    }

    app.width = 800;
    app.height = 600;
    const ctx = app.getContext("2d");
    const w = await WebAssembly.instantiateStreaming(fetch(wasmPath), {
        "env": make_environment(libm)
    });

    let prev = null;
    function first(timestamp) {
        prev = timestamp;
        window.requestAnimationFrame(loop);
    }
    function loop(timestamp) {
        const dt = timestamp - prev;
        prev = timestamp;

        const pixels = w.instance.exports.render(dt*0.001);
        const buffer = w.instance.exports.memory.buffer;
        const image = new ImageData(new Uint8ClampedArray(buffer, pixels, app.width*app.height*4), app.width);
        ctx.putImageData(image, 0, 0);
        window.requestAnimationFrame(loop);
    }
    window.requestAnimationFrame(first);
}
