// TODO: Hook-up to actual micro:bit using serial code:
// (Test serialport code below)
//
//if (process.env.BROWSER_SYNC_CLIENT_URL) {
//  const current = document.currentScript;
//  const script = document.createElement('script');
//  script.src = process.env.BROWSER_SYNC_CLIENT_URL;
//  script.async = true;
//  current.parentNode.insertBefore(script, current);
//}

////Print out ports to dev console
//serialport.list((err, ports) => {
//  for (var i = 0; i < ports.length; i++) {
//    console.log(ports[i].comName);
//  }
//});

//var port = new serialport('/dev/ttyACM0', { autoOpen: true, baudRate : 115200 });
//port.write('test\n', function(err) {
//  if (err) {
//    console.log(err);
//    return;
//  }
//  console.log("Message written");
//});

const randomInteger = (start, end) => {
  return Math.floor(Math.random() * (1 - start + end)) + 1;
};

const getGraph = () => {
  return {
    nodes: [
      {id: 1, label: 'Node 1'},
      {id: 2, label: 'Node 2'},
      {id: 3, label: 'Node 3'},
      {id: 4, label: 'Node 4'},
      {id: 5, label: 'Node 5'}
    ],
    edges: [
      {from: randomInteger(1, 5), to: randomInteger(1, 5)},
      {from: randomInteger(1, 5), to: randomInteger(1, 5)},
      {from: randomInteger(1, 5), to: randomInteger(1, 5)},
      {from: randomInteger(1, 5), to: randomInteger(1, 5)},
      {from: randomInteger(1, 5), to: randomInteger(1, 5)},
      {from: randomInteger(1, 5), to: randomInteger(1, 5)},
      {from: randomInteger(1, 5), to: randomInteger(1, 5)}
    ]
  };
};

export { getGraph };
