
# get-adapter-info

### network adapter lookup with nodejs

* get the currently connected network adapter
* Only supports window7+ systems

## Example

### Code
~~~js
const connectedAdapterInfo = require("get-adapter-info")
console.log(connectedAdapterInfo())
~~~

### Result Object
~~~js
{
    bDescr: 'Intel(R) Wi-Fi 6 AX201 160MHz',
    dwInOctets: 128230073,
    dwOutOctets: 34135504,
    wszName: '\\DEVICE\\TCPIP_{488E36CC-E8A2-4812-8BD7-8399CBB0D486}',
    dwType: 71,
    dwOperStatus: 5,
    dwAdminStatus: 1,
    dwSpeed: 144400000
}

~~~
