// ESLint flat config (ESLint 9 and later read no .eslintrc file).
// eslint-config-google predates flat config and names two rules ESLint has
// since removed (valid-jsdoc, require-jsdoc), so only the rules ESLint still
// ships are taken from it.
const google = require("eslint-config-google");
const { builtinRules } = require("eslint/use-at-your-own-risk");

const googleRules = Object.fromEntries(
  Object.entries(google.rules).filter(([name]) => builtinRules.has(name)),
);

module.exports = [
  {
    files: ["fsharp/grammar.js", "fsharp_signature/grammar.js"],
    languageOptions: {
      ecmaVersion: "latest",
      sourceType: "commonjs",
    },
    linterOptions: {
      reportUnusedDisableDirectives: "off",
    },
    rules: {
      ...googleRules,
      // The grammars use double quotes, single ones around a `"`.
      "quotes": ["error", "double", { avoidEscape: true, allowTemplateLiterals: true }],
      "indent": ["error", 2, { SwitchCase: 1 }],
      // A token's regular expression cannot be split over lines.
      "max-len": [
        "error",
        {
          code: 120,
          ignoreComments: true,
          ignoreUrls: true,
          ignoreStrings: true,
          ignoreRegExpLiterals: true,
        },
      ],
      "no-multi-spaces": ["error", { ignoreEOLComments: true }],
      "operator-linebreak": [
        "error",
        "after",
        { overrides: { "?": "before", ":": "before" } },
      ],
    },
  },
];
