const http = require('http');

const options = {
  hostname: '127.0.0.1',
  port: 8082,
  path: '/',
  method: 'POST',
  headers: {
    'Transfer-Encoding': 'chunked'
  }
};

const req = http.request(options, (res) => {
  res.on('data', (chunk) => {
    console.log(`Response: ${chunk}`);
  });
});

req.write('7\r\nMozilla\r\n');
req.write('9\r\nDeveloper\r\n');
req.write('7\r\nNetwork\r\n');
req.write('0\r\n\r\n');
req.end();


/*


(
  echo -ne "POST / HTTP/1.1\r\n"
  echo -ne "Host: 127.0.0.1:8082\r\n"
  echo -ne "Transfer-Encoding: chunked\r\n"
  echo -ne "\r\n"
  echo -ne "7\r\nMozilla\r\n"
  echo -ne "9\r\nDeveloper\r\n"
  echo -ne "7\r\nNetwork\r\n"
  echo -ne "0\r\n\r\n"
) | nc 127.0.0.1 8082



*/ //Curl makatencodich req nichan ila drna te: chuncked...