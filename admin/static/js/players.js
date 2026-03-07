(function() {
  var container = document.querySelector("[data-status-url]");
  if (!container) return;
  var url = container.getAttribute("data-status-url");
  var STATUS_CLASSES = {online: "badge-success", stale: "badge-warning", offline: "badge-info"};
  function refreshStatus() {
    fetch(url)
      .then(function(r) {
        if (!r.ok) throw new Error("status " + r.status);
        return r.json();
      })
      .then(function(data) {
        document.querySelectorAll("[data-player]").forEach(function(el) {
          var name = el.getAttribute("data-player");
          var status = data[name] || "offline";
          el.textContent = status.charAt(0).toUpperCase() + status.slice(1);
          el.className = "badge " + (STATUS_CLASSES[status] || "badge-info");

          var editEl = document.querySelector('[data-edit="' + name + '"]');
          if (editEl) {
            if (status === "offline") {
              if (editEl.tagName !== "A") {
                var a = document.createElement("a");
                a.href = editEl.getAttribute("data-href") || ("players/" + name);
                a.className = "btn btn-sm";
                a.setAttribute("data-edit", name);
                a.textContent = "Edit";
                editEl.replaceWith(a);
              }
            } else {
              if (editEl.tagName !== "SPAN") {
                var span = document.createElement("span");
                span.className = "btn btn-sm disabled";
                span.setAttribute("data-edit", name);
                span.setAttribute("data-href", editEl.href);
                span.textContent = "Edit";
                editEl.replaceWith(span);
              }
            }
          }
        });
      })
      .catch(function(e) { console.warn("status poll:", e); });
  }
  setInterval(refreshStatus, 10000);
})();
