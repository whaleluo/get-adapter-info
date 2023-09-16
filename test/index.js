var index = require('bindings')('getAdapterInfo');
var lastIn=1,lastOut=1
// setInterval(()=>{
    const cc = index()
// const cc = addon()

     console.log(cc)
    // console.log((cc.dwInOctets-lastIn)/1024)
    lastIn = cc.dwInOctets
    lastOut = cc.dwOutOctets
// },1000)
