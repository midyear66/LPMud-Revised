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
        });
      })
      .catch(function(e) { console.warn("status poll:", e); });
  }
  setInterval(refreshStatus, 10000);
})();
