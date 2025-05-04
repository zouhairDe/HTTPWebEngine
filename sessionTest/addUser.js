#!/usr/bin/env node

const fs = require('fs');

const SESSIONS_DB = "/tmp/sessions.db";
const SESSION_DURATION_MS = 60 * 1000; // 1 minutes

function generateSessionID() {
  return Math.random().toString(36).substring(2, 18);
}

function parseForm(data) {
  const result = {};
  data.split("&").forEach(pair => {
    const [key, val] = pair.split("=").map(decodeURIComponent);
    result[key] = val.replace(/\+/g, " ");
  });
  return result;
}

function getPostData(callback) {
  const length = parseInt(process.env.CONTENT_LENGTH || "0", 10);
  if (!length) return callback({});
  let body = '';
  process.stdin.setEncoding('utf8');
  process.stdin.on('data', chunk => body += chunk);
  process.stdin.on('end', () => {
    callback(parseForm(body));
  });
}

function loadSessions() {
  if (!fs.existsSync(SESSIONS_DB)) return [];
  return fs.readFileSync(SESSIONS_DB, 'utf8').split('\n').filter(Boolean).map(line => {
    const [id, username, role, expires] = line.split('|');
    return { id, username, role, expires: parseInt(expires) };
  });
}

function saveSessions(sessions) {
  const lines = sessions.map(s => `${s.id}|${s.username}|${s.role}|${s.expires}`);
  fs.writeFileSync(SESSIONS_DB, lines.join('\n'));
}

function outputCookieHeader(id) {
  console.log("HTTP/1.1 200 OK");
  console.log(`Set-Cookie: session_id=${id}; Path=/; HttpOnly`);
  console.log("Content-Type: text/plain\n");
  console.log(`session_id=${id}`);
}

getPostData(({ username, role }) => {
  if (!username || !role) {
    console.log("HTTP/1.1 400 Bad Request");
    console.log("Content-Type: text/plain\n");
    console.log("Missing username or role");
    return;
  }

  let sessions = loadSessions();
  const now = Date.now();

  let existing = sessions.find(s => s.username === username && s.role === role);
  if (existing) {
    if (now > existing.expires) {
      existing.expires = now + SESSION_DURATION_MS;
    }
    outputCookieHeader(existing.id);
  } else {
    const id = generateSessionID();
    sessions.push({ id, username, role, expires: now + SESSION_DURATION_MS });
    outputCookieHeader(id);
  }

  saveSessions(sessions);
});
