#!/usr/bin/env node
// npm renders whatever README.md sits at the root of the published tarball, and has no
// package.json field to point somewhere else. So `prepack` swaps the Node binding's
// README into place and `postpack` puts the repository README back.
//
// Cargo (`readme` in Cargo.toml) and PyPI (`readme` in pyproject.toml) point at their
// own binding READMEs directly and need no such dance.

const fs = require("node:fs");
const path = require("node:path");

const root = path.join(__dirname, "..");
const repoReadme = path.join(root, "README.md");
const nodeReadme = path.join(root, "bindings", "node", "README.md");
// Must NOT be named README*: npm force-includes every README* in the tarball,
// regardless of the `files` whitelist, and the backup would ship with it.
const backup = path.join(root, ".npm-readme-backup");

const mode = process.argv[2];

if (mode === "swap") {
  if (fs.existsSync(backup)) {
    throw new Error(
      `${backup} already exists — an earlier pack did not finish. ` +
        "Restore README.md from it (or from git) and delete the backup.",
    );
  }
  fs.copyFileSync(repoReadme, backup);
  fs.copyFileSync(nodeReadme, repoReadme);
} else if (mode === "restore") {
  if (fs.existsSync(backup)) {
    fs.copyFileSync(backup, repoReadme);
    fs.rmSync(backup);
  }
} else {
  throw new Error(`usage: npm-readme.js <swap|restore> (got ${mode ?? "nothing"})`);
}
