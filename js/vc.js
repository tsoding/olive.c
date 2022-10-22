// Browser runtime for the Demo Virtual Console
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

let iota = 0;
// TODO: nothing in this Canvas "declaration" states that iota's measure units are Uint32
// Which is not useful for all kinds of structures. A more general approach would be to use Uint8 as the measure units.
const CANVAS_PIXELS = iota++;
const CANVAS_WIDTH  = iota++;
const CANVAS_HEIGHT = iota++;
const CANVAS_STRIDE = iota++;
const CANVAS_SIZE   = iota++;

function readCanvasFromMemory(memory_buffer, canvas_ptr)
{
    const canvas_memory = new Uint32Array(memory_buffer, canvas_ptr, CANVAS_SIZE);
    return {
        pixels: canvas_memory[CANVAS_PIXELS],
        width: canvas_memory[CANVAS_WIDTH],
        height: canvas_memory[CANVAS_HEIGHT],
        stride: canvas_memory[CANVAS_STRIDE],
    };
}

async function startDemo(elementId, wasmPath) {
    const app = document.getElementById(elementId);
    if (app === null) {
        console.error(`Could not find element ${elementId}. Skipping demo ${wasmPath}...`);
        return;
    }
    loadApp(app, wasmPath);
}

async function loadApp(app, wasmPath) {
    const ctx = app.getContext("2d");
    const w = await WebAssembly.instantiateStreaming(fetch(wasmPath), {
        "env": make_environment(libm)
    });

    // TODO: if __heap_base not found tell the user to compile their wasm module with -Wl,--export=__heap_base
    const heap_base = w.instance.exports.__heap_base.value;

    let prev = null;
    function first(timestamp) {
        prev = timestamp;
        window.requestAnimationFrame(loop);
    }
    function loop(timestamp) {
        const dt = timestamp - prev;
        prev = timestamp;

        const buffer = w.instance.exports.memory.buffer;
        w.instance.exports.render(heap_base, dt*0.001);
        const canvas = readCanvasFromMemory(buffer, heap_base);
        if (canvas.width != canvas.stride) {
            // TODO: maybe we can preallocate a Uint8ClampedArray on JavaScript side and just copy the canvas data there to bring width and stride to the same value?
            console.error(`Canvas width (${canvas.width}) is not equal to its stride (${canvas.stride}). Unfortunately we can't easily support that in a browser because ImageData simply does not accept stride. Welcome to 2022.`);
            return;
        }
        const image = new ImageData(new Uint8ClampedArray(buffer, canvas.pixels, canvas.width*canvas.height*4), canvas.width);
        app.width = canvas.width;
        app.height = canvas.height;
        ctx.putImageData(image, 0, 0);
        window.requestAnimationFrame(loop);
    }
    window.requestAnimationFrame(first);
}

class OliveCanvas extends HTMLElement {
    constructor() {
        super();
    }

    connectedCallback() {
        const wasmPath = this.getAttribute('src');
        if (!wasmPath) {
            console.error(`Should define a src attribute on olive-canvas`)
            return;
        }

        const canvas = document.createElement("canvas");
        this.appendChild(canvas);
        loadApp(canvas, wasmPath);
    }
}

customElements.define("olivec-canvas", OliveCanvas);
