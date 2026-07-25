const VERSION_RE = /(project\(uuid_ext VERSION )\d+\.\d+\.\d+(\))/;

module.exports.readVersion = function (contents) {
  return contents.match(VERSION_RE)[0].replace(/^project\(uuid_ext VERSION /, '').replace(/\)$/, '');
};

module.exports.writeVersion = function (contents, version) {
  return contents.replace(VERSION_RE, `$1${version}$2`);
};
