let app = document.getElementById("app");
app.width = 800;
app.height = 600;
let ctx = app.getContext("2d");
let w = null;

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

// TODO: deploy the WASM examples to GitHub pages
// TODO: display all the VC examples on a single page
WebAssembly.instantiateStreaming(fetch('./build/squish.wasm'), {
    "env": make_environment({
        "atan2f": Math.atan2,
        "cosf": Math.cos,
        "sinf": Math.sin,
        "sqrtf": Math.sqrt,
    })
}).then(w0 => {
    w = w0;

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
    w.instance.exports.init();
    window.requestAnimationFrame(first);
})
