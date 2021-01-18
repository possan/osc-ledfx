export function cleanDocument(input) {
  const cleandoc = {
    layers: [],
  };

  input.layers.forEach((obj) => {
    console.log("obj", obj);
    const cleanlayer = {
      id: obj.id,
      start: obj.start,
      end: obj.end,
      collapsed: obj.collapsed,
      // properties: obj.properties,
      values: obj.values,
      properties: [],
    };
    var bykey = {};
    obj.properties.forEach((prop) => {
      // console.log('prop', prop)
      bykey[prop.name] = prop;

      const cleankeys = [];
      if (prop.keys) {
        prop.keys.forEach((key) => {
          cleankeys.push({
            time: key.time,
            val: key.val,
            ease: key.ease,
          });
        });
      }
      const cleanprop = {
        name: prop.name,
        keys: cleankeys,
      };
      cleanlayer.properties.push(cleanprop);
    });
    console.log("prop", bykey);
    cleandoc.layers.push(cleanlayer);
  });

  return cleandoc;
}
