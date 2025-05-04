#!/usr/bin/env node

const fs = require('fs');
function parseCookies(raw) {
  const out = {};
  const parts = raw.split(';');
  for (let i = 0; i < parts.length; i++) {
    const kv = parts[i].split('=');
    if (kv.length === 2) {
      const key = kv[0].trim();
      const value = kv[1].trim();
      out[key] = value;
    }
  }
  return out;
}

let name ;

const sessionFile = '/tmp/sessions.db';

function isSessionValid(sessionId) {
  if (!fs.existsSync(sessionFile)) return false;
  const lines = fs.readFileSync(sessionFile, 'utf8').split('\n');
  for (let line of lines) {
    const [id, username, role, expires] = line.split('|');
    if (id === sessionId && (!expires || Date.now() < Number(expires))) {
      name = username;
      return true;
    }
  }
  return false;
}

function outputHTML(body, status = "200 OK") {
  console.log(`HTTP/1.1 ${status}`);
  console.log("Content-Type: text/html");
  console.log("Content-Length: " + Buffer.byteLength(body));
  console.log("");
  console.log(body);
}

const rawCookies = process.env.HTTP_COOKIE || '';
const cookiesParsed = parseCookies(rawCookies);

const sessionId = cookiesParsed['session_id'];

if (sessionId && isSessionValid(sessionId)) {
  outputHTML(`<!DOCTYPE html><html><head><title>Welcome back ${name} </title></head><body><h1>Welcome back ${name}!</h1><p>You are authenticated.</p></body></html>`);
} else {
  outputHTML("<!DOCTYPE html><html><head><title>Unauthorized</title></head><body><h1>401 Unauthorized</h1><p>Please log in.</p></body></html>", "401 Unauthorized");
}
