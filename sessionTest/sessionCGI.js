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

function outputWelcomeHTML(name) {
  const body = `
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8" />
    <title>Welcome ${name}</title>
    <style>
      body {
        font-family: 'Segoe UI', sans-serif;
        background: #f0f2f5;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        height: 100vh;
        margin: 0;
      }
      .card {
        background: white;
        padding: 2rem;
        border-radius: 12px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.1);
        text-align: center;
      }
      h1 {
        color: #333;
      }
      p {
        color: #666;
      }
      button {
        margin-top: 1rem;
        padding: 0.6rem 1.2rem;
        background: #007bff;
        color: white;
        border: none;
        border-radius: 6px;
        font-size: 1rem;
        cursor: pointer;
        transition: background 0.2s ease;
      }
      button:hover {
        background: #0056b3;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Welcome back, ${name}!</h1>
      <p>You are authenticated.</p>
      <button onclick="window.location.href='/login'">Logout</button>
    </div>
  </body>
  </html>
  `;
  outputHTML(body);
}

function outputUnauthorizedHTML() {
  const body = `
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8" />
    <title>No session for you</title>
    <style>
      body {
        font-family: 'Segoe UI', sans-serif;
        background: #fff3f3;
        color: #a94442;
        display: flex;
        align-items: center;
        justify-content: center;
        height: 100vh;
        margin: 0;
      }
      .card {
        text-align: center;
        border: 1px solid #ebccd1;
        padding: 2rem;
        background: #f2dede;
        border-radius: 10px;
      }
      h1 {
        font-size: 2rem;
        margin-bottom: 1rem;
      }
        button {
        margin-top: 1rem;
        padding: 0.6rem 1.2rem;
        background: #007bff;
        color: white;
        border: none;
        border-radius: 6px;
        font-size: 1rem;
        cursor: pointer;
        transition: background 0.2s ease;
      }
      button:hover {
        background: #0056b3;
      }
      p {
        font-size: 1rem;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>No session for you</h1>
      <p>Please log in.</p>
      <button onclick="window.location.href='/login'">Logout</button>
    </div>
  </body>
  </html>
  `;
  outputHTML(body, "401 Unauthorized");
}


if (sessionId && isSessionValid(sessionId)) {
  outputWelcomeHTML(name);
} else {
  outputUnauthorizedHTML();}
