document.addEventListener('DOMContentLoaded', function() {
  // Confirm dialogs for forms with data-confirm attribute
  document.addEventListener('submit', function(e) {
    var form = e.target;
    var message = form.getAttribute('data-confirm');
    if (message && !confirm(message)) {
      e.preventDefault();
    }
  });

  // Loading state for buttons with data-loading attribute
  document.addEventListener('click', function(e) {
    var btn = e.target.closest('button[data-loading]');
    if (btn) {
      var text = btn.getAttribute('data-loading');
      btn.disabled = true;
      btn.textContent = text;
      btn.form.submit();
    }
  });

  // Popup window for elements with data-popup attribute
  document.addEventListener('click', function(e) {
    var el = e.target.closest('[data-popup]');
    if (el) {
      e.preventDefault();
      window.open(
        el.getAttribute('data-popup'),
        'mudclient',
        'width=900,height=600,menubar=no,toolbar=no,location=no,status=no'
      );
    }
  });
});
