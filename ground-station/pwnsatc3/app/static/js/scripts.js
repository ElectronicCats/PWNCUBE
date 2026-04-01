setInterval(currentTIme, 1000);

function currentTIme() {
  const d = new Date();
  document.getElementById("timeNavbar").innerHTML = `UTC: ${d.toUTCString()}`;
}

function createKeyValueRow(key, value) {
  let html = '';
  let normalizedKey = key.replace(/_/g, ' ');
  normalizedKey = normalizedKey.charAt(0).toUpperCase() + normalizedKey.slice(1);
  if (typeof value === Object || typeof value === Array) {
    return html;
  }
  html = ` <tr>
              <th class="text-capitalize">${normalizedKey}:</th>
              <td>${value}</td>
            </tr>`;
  return html;
}

function createInputField(key, value) {
  let html = '';
  let normalizedKey = key.replace(/_/g, ' ');
  normalizedKey = normalizedKey.charAt(0).toUpperCase() + normalizedKey.slice(1);
  if (typeof value === Object || typeof value === Array) {
    return html;
  }
  html = `<div class="col-md-6">
            <label for="${key}" class="form-label text-capitalize">${normalizedKey}</label>
            <input type="text" class="form-control" id="input_${key}" name="${key}" value="${value}">
          </div>`;
  return html;
}

function createInputParameter(param, name) {
  let html = `<div class="d-flex justify-content-between">
              <p>Description: ${param["description"]}</p>`;
  if (param["inttype"] != "STRING") {
    html += `<p><span class="text-yellow">Min: ${param["min"]}</span> - <span class="text-red">Max: ${param["max"]}</span></p>`;
  }
  html += `</div>`;
  html += createInputField(name, param["value"]);
  return html;
}