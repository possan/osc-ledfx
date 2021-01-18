import { Timeliner } from './timeliner.min.js'

window.addEventListener('load', () => {
    var target = {
        layer_0_opacity: 1,
        layer_0_color_0: 2,
        layer_0_color_1: 3,
        layer_0_color_2: 1,
        layer_0_colorspace: 2,
        layer_0_offset: 3,
        layer_0_size: 1,
        layer_0_repeat: 2,
        layer_0_feather_left: 3,
        layer_0_feather_right: 1,
        layer_0_speed_multiplier: 2,
    }

    // var dummyload = {
    //     "version": "test-version",
    //     "modified": "Wed Jan 13 2021 23:00:59 GMT+0100 (Central European Standard Time)",
    //     "title": "Untitled",
    //     "ui": {
    //       "currentTime": 9,
    //       "totalTime": 20,
    //       "scrollTime": 0,
    //       "timeScale": 6
    //     },
    //     "layers": [
    //       {
    //         "name": "name1",
    //         "values": [
    //           {
    //             "time": 0,
    //             "value": -1.2000000000000002,
    //             "_color": "#200f29",
    //             "tween": "quadEaseOut"
    //           },
    //           {
    //             "time": 10,
    //             "value": 4.2,
    //             "_color": "#a98b3"
    //           }
    //         ],
    //         "_value": 4.146000000000001,
    //         "_color": "#9032d6",
    //         "_mute": false
    //       },
    //       {
    //         "name": "name2",
    //         "values": [
    //           {
    //             "time": 4,
    //             "value": 11.4,
    //             "_color": "#139450",
    //             "tween": "quadEaseIn"
    //           },
    //           {
    //             "time": 19,
    //             "value": -6.314000000000004,
    //             "_color": "#9ed444"
    //           }
    //         ],
    //         "_value": 9.431777777777778,
    //         "_color": "#287456",
    //         "_solo": false,
    //         "_mute": false
    //       }
    //     ]
    //   }

    var timeliner = new Timeliner(target)

    timeliner.addLayer('1.Opacity')
    timeliner.addLayer('1.Red/Hue')
    timeliner.addLayer('1.Grn/Sat')
    timeliner.addLayer('1.Blu/Val')
    timeliner.addLayer('1.RGB/HSV')
    timeliner.addLayer('1.Offset')
    timeliner.addLayer('1.Size')
    timeliner.addLayer('1.Repeat')
    timeliner.addLayer('1.Fade L')
    timeliner.addLayer('1.Fade R')
    timeliner.addLayer('1.Speed Mul')
    timeliner.addLayer('2.Opacity')
    timeliner.addLayer('2.Red/Hue')
    timeliner.addLayer('2.Grn/Sat')
    timeliner.addLayer('2.Blu/Val')
    timeliner.addLayer('2.RGB/HSV')
    timeliner.addLayer('2.Offset')
    timeliner.addLayer('2.Size')
    timeliner.addLayer('2.Repeat')
    timeliner.addLayer('2.Fade L')
    timeliner.addLayer('2.Fade R')
    timeliner.addLayer('2.Speed Mul')
    timeliner.addLayer('3.Opacity')
    timeliner.addLayer('3.Red/Hue')
    timeliner.addLayer('3.Grn/Sat')
    timeliner.addLayer('3.Blu/Val')
    timeliner.addLayer('3.RGB/HSV')
    timeliner.addLayer('3.Offset')
    timeliner.addLayer('3.Size')
    timeliner.addLayer('3.Repeat')
    timeliner.addLayer('3.Fade L')
    timeliner.addLayer('3.Fade R')
    timeliner.addLayer('3.Speed Mul')

    // timeliner.load(dummyload) // funkar!

    var lastjson = ''
    setInterval(() => {
        // var json = JSON.stringify(window._data.getJSONString('\t'))
        var json = JSON.stringify(window._data.data, null, 2)
        if (json != lastjson) {
            lastjson = json
            document.getElementById('jsondump').textContent = json
        }
    }, 500)

    window.timeliner = timeliner

})
