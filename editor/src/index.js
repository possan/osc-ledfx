// import { Timeliner } from './timeliner.min.js'

import EMPTY_DOCUMENT from "./emptyscene.js";
import { cleanDocument } from './doc-utils.js'

const GLOB_NUM_LEDS = 1;
const GLOB_PIXELORDER = 2;
const GLOB_NUDGE = 3;
const GLOB_OPACITY = 4;
const GLOB_BASE_SPEED = 5;
const LAYER1_OFFSET = 0;
const LAYER2_OFFSET = 50;
const LAYER3_OFFSET = 100;
// const LAYER4_OFFSET = 150;
const LAYER_OPACITY = 10;
const LAYER_OFFSET = 11;
const LAYER_SIZE = 12;
const LAYER_REPEAT = 13;
const LAYER_FEATHER_LEFT = 14;
const LAYER_FEATHER_RIGHT = 15;
const LAYER_SPEED_MULTIPLIER = 16;
const LAYER_BLEND = 17;
const LAYER_COLOR_R = 20;
const LAYER_COLOR_G = 21;
const LAYER_COLOR_B = 22;
const LAYER_COLORSPACE = 23;

window.addEventListener("load", () => {
  let ledBuf;
  let fx_binding_reset;
  let fx_binding_set_property;
  let fx_binding_load_animation;
  let fx_binding_render;

  let tweenCore, tweenEditor

  const MAXLEDS = 36*11;
  let pixeldata = new Array(3 * MAXLEDS);

  let canvas;
  let ctx;

  let data;

  let StartTime = Date.now();
  let CurrentTime = Date.now();

  setTimeout(() => {
    console.log("Module", Module);

    fx_binding_reset = Module.cwrap("fx_binding_reset", "void", []);

    fx_binding_set_property = Module.cwrap("fx_binding_set_property", "void", [
      "number",
      "number",
    ]);

    fx_binding_load_animation = Module.cwrap(
      "fx_binding_load_animation",
      "void",
      ["number", "number"]
    );

    fx_binding_render = Module.cwrap("fx_binding_render", "void", [
      "number",
      "number",
      "number",
      "number",
      "number",
    ]);

    ledBuf = Module._malloc(100000);

    fx_binding_reset();
    console.log("resetted.");

    fx_binding_set_property(GLOB_NUM_LEDS, MAXLEDS);
    fx_binding_set_property(GLOB_PIXELORDER, 0);
    fx_binding_set_property(GLOB_OPACITY, 100);
    fx_binding_set_property(GLOB_BASE_SPEED, 1000);

    // fx_binding_set_property(LAYER1_OFFSET + LAYER_OPACITY, 100);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_OFFSET, 0);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLOR_R, 350);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLOR_G, 100);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLOR_B, 100);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLORSPACE, 1);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_SIZE, 20);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_REPEAT, 1000);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_FEATHER_LEFT, 20);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_FEATHER_RIGHT, 20);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_SPEED_MULTIPLIER, 3000);
    // fx_binding_set_property(LAYER1_OFFSET + LAYER_BLEND, 0);

    // fx_binding_set_property(LAYER2_OFFSET + LAYER_OPACITY, 0);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_OFFSET, 0);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLOR_R, 255);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLOR_G, 0);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLOR_B, 100);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLORSPACE, 1);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_SIZE, 30);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_REPEAT, 1000);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_FEATHER_LEFT, 10);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_FEATHER_RIGHT, 10);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER, -2000);
    // fx_binding_set_property(LAYER2_OFFSET + LAYER_BLEND, 1);

    // fx_binding_render(1000, ledBuf, 20)
    // let ledBufData = Module.HEAPU8.subarray(ledBuf, ledBuf + 3*20)
    // console.log(ledBuf, ledBufData)

    // fx_binding_render(2000, ledBuf, 20)
    // ledBufData = Module.HEAPU8.subarray(ledBuf, ledBuf + 3*20)
    // console.log(ledBuf, ledBufData)

    // fx_binding_render(3000, ledBuf, 20)
    // ledBufData = Module.HEAPU8.subarray(ledBuf, ledBuf + 3*20)
    // console.log(ledBuf, ledBufData)

    renderFrame();
  }, 2000);

  function resetDocument() {
  }

  document.getElementById('load').addEventListener('click', () => {
    const json = prompt('Paste some json here..')
    if (json !== undefined) {
      resetDocument()
      const parsed = JSON.parse(json)
      console.log('pasted json', parsed)
      data.layers = parsed.layers
      tweenCore.setData(data.layers)
      tweenEditor.forceItemsRender()
    }
  })

  document.getElementById('save').addEventListener('click', () => {
    const json = persist();

    var data = [
      new ClipboardItem({
        "text/plain": new Blob([json], { type: "text/plain" })
      })
    ];

    navigator.clipboard.write(data).then(function() {
      console.log("Copied to clipboard successfully!");
      alert('JSON blob copied to clipboard.');
    }, function() {
      console.error("Unable to write to clipboard. :-(");
    });
  })

  document.getElementById('reset').addEventListener('click', () => {
    alert('I am too lazy, just reload the browser')
  })

  canvas = document.getElementById("leds");
  canvas.width = 800;
  canvas.height = 400;
  ctx = canvas.getContext("2d");

  updateLedPreview();

  function updateLedPreview() {
    // console.log('update led preview', pixeldata)

    ctx.fillStyle = "#000";
    ctx.clearRect(0, 0, 800, 400);

    for (var s = 0; s < 11; s++) {
      for (var i = 0; i < 36; i++) {
        var o = s * 36 + i;
        var r = pixeldata[o * 3 + 0];
        var g = pixeldata[o * 3 + 1];
        var b = pixeldata[o * 3 + 2];

        var x = 10 + i * 8;
        var y = 10 + s * 30;
        ctx.fillStyle = `rgb(${r},${g},${b})`;
        ctx.fillRect(x, y, 6, 28);

        var a1 = (s * 360) / 11;
        var a2 = a1 + 15 + 90;

        var r1 = (a1 * Math.PI) / 180;
        var r2 = (a2 * Math.PI) / 180;
        var id = i * 2.3;

        var x2 = 500 + 150 * Math.cos(r1) + id * Math.cos(r2);
        var y2 = 200 + 150 * Math.sin(r1) + id * Math.sin(r2);

        ctx.fillRect(x2, y2, 3, 3);
      }
    }
  }

  let lastT = Date.now() - StartTime

  function renderFrame() {
    let T = Date.now() - StartTime;
    let dT = T - lastT
    lastT = T

    // console.log('render', T, dT)

    fx_binding_render(T, dT, ledBuf, MAXLEDS);
    let ledBufData = Module.HEAPU8.subarray(ledBuf, ledBuf + 3 * MAXLEDS);

    // console.log(ledBuf, ledBufData)

    for (var j = 0; j < 3 * MAXLEDS; j++) {
      pixeldata[j] = ledBufData[j];
    }

    updateLedPreview();

    CurrentTime += 50;
    setTimeout(() => renderFrame(), 50);
  }

  data = Object.assign({}, EMPTY_DOCUMENT);
  console.log('data', data)

  tweenCore = new TweenTime.Core(data.layers, {
    defaultEase: "Linear.easeNone",
  });
  tweenCore.orchestrator.onUpdate.add(debouncePersist.bind(this));

  tweenEditor = new TweenTime.Editor(tweenCore);
  tweenEditor.propertiesEditor.keyAdded.add(debouncePersist.bind(this));
  tweenEditor.propertiesEditor.keyRemoved.add(debouncePersist.bind(this));
  // edi/r.keys.dated.add(debouncePersist.bind(this))
  // editor.curves.onCurveUpdated.add(debouncePersist.bind(this))
  tweenEditor.selectionManager.onSelect.add(debouncePersist.bind(this));

  console.log("editor", tweenEditor);

  var debouncePersistTimer = 0;
  function debouncePersist() {
    if (debouncePersistTimer) {
      clearTimeout(debouncePersistTimer);
    }
    debouncePersistTimer = setTimeout(() => persist(), 500);
  }

  function persist() {
    var tmplayers = tweenCore.getData()
    var tmpdata = {
      layers: tmplayers
    }
    console.log("persist document", tmpdata)

    const cleandoc = cleanDocument(tmpdata)
    console.log('clean doc', cleandoc)

    const json = JSON.stringify(cleandoc)
    document.getElementById("jsondump").value = json
    return json
  }

  function animate() {
    const l1 = tweenCore.getValues("layer1");
    const l2 = tweenCore.getValues("layer2");
    const l3 = tweenCore.getValues("layer3");
    // console.log('layers', l1, l2, l3, data)

    if (fx_binding_set_property && l1 && l2 && l3) {
      // fx_binding_set_property(GLOB_NUM_LEDS, MAXLEDS);
      // fx_binding_set_property(GLOB_PIXELORDER, 0);
      // fx_binding_set_property(GLOB_OPACITY, 100);
      // fx_binding_set_property(GLOB_BASE_SPEED, 1000);

      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_OPACITY,
        Math.round(l1.opacity)
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_OFFSET,
        Math.round(l1.offset)
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_COLOR_R,
        Math.round(l1.red)
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_COLOR_G,
        Math.round(l1.green)
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_COLOR_B,
        Math.round(l1.blue)
      );
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLOR_R, 350);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLOR_G, 100);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLOR_B, 100);
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_COLORSPACE,
        Math.round(l1.colorspace)
      );
      fx_binding_set_property(LAYER1_OFFSET + LAYER_SIZE, Math.round(l1.size));
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_REPEAT,
        Math.round(l1.repeat)
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_FEATHER_LEFT,
        Math.round(l1["feather-left"])
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_FEATHER_RIGHT,
        Math.round(l1["feather-right"])
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_SPEED_MULTIPLIER,
        Math.round(l1["speed-multiplier"])
      );
      fx_binding_set_property(
        LAYER1_OFFSET + LAYER_BLEND,
        Math.round(l1.blending)
      );
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_COLORSPACE, 1);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_SIZE, 20);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_REPEAT, 1);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_FEATHER_LEFT, 20);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_FEATHER_RIGHT, 20);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_SPEED_MULTIPLIER, 3000);
      // fx_binding_set_property(LAYER1_OFFSET + LAYER_BLEND, 0);

      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_OPACITY,
        Math.round(l2.opacity)
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_OFFSET,
        Math.round(l2.offset)
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_COLOR_R,
        Math.round(l2.red)
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_COLOR_G,
        Math.round(l2.green)
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_COLOR_B,
        Math.round(l2.blue)
      );
      // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLOR_R, 350);
      // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLOR_G, 100);
      // fx_binding_set_property(LAYER2_OFFSET + LAYER_COLOR_B, 100);
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_COLORSPACE,
        Math.round(l2.colorspace)
      );
      fx_binding_set_property(LAYER2_OFFSET + LAYER_SIZE, Math.round(l2.size));
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_REPEAT,
        Math.round(l2.repeat)
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_FEATHER_LEFT,
        Math.round(l2["feather-left"])
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_FEATHER_RIGHT,
        Math.round(l2["feather-right"])
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_SPEED_MULTIPLIER,
        Math.round(l2["speed-multiplier"])
      );
      fx_binding_set_property(
        LAYER2_OFFSET + LAYER_BLEND,
        Math.round(l2.blending)
      );

      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_OPACITY,
        Math.round(l3.opacity)
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_OFFSET,
        Math.round(l3.offset)
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_COLOR_R,
        Math.round(l3.red)
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_COLOR_G,
        Math.round(l3.green)
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_COLOR_B,
        Math.round(l3.blue)
      );
      // fx_binding_set_property(LAYER3_OFFSET + LAYER_COLOR_R, 350);
      // fx_binding_set_property(LAYER3_OFFSET + LAYER_COLOR_G, 100);
      // fx_binding_set_property(LAYER3_OFFSET + LAYER_COLOR_B, 100);
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_COLORSPACE,
        Math.round(l3.colorspace)
      );
      fx_binding_set_property(LAYER3_OFFSET + LAYER_SIZE, Math.round(l3.size));
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_REPEAT,
        Math.round(l3.repeat)
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_FEATHER_LEFT,
        Math.round(l3["feather-left"])
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_FEATHER_RIGHT,
        Math.round(l3["feather-right"])
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_SPEED_MULTIPLIER,
        Math.round(l3["speed-multiplier"])
      );
      fx_binding_set_property(
        LAYER3_OFFSET + LAYER_BLEND,
        Math.round(l3.blending)
      );
    }

    setTimeout(() => animate(), 100);
  }

  animate();
  debouncePersist();
});
