// Light/dark theme toggle with system preference detection
(function () {
  const html = document.documentElement;
  const toggle = document.getElementById("theme-toggle");
  const STORAGE_KEY = "lpmud-admin-theme";

  function getSystemTheme() {
    return window.matchMedia("(prefers-color-scheme: light)").matches
      ? "light"
      : "dark";
  }

  function applyTheme(theme) {
    html.setAttribute("data-theme", theme);
  }

  // Initialize: saved preference > system preference
  const saved = localStorage.getItem(STORAGE_KEY);
  applyTheme(saved || getSystemTheme());

  // Toggle button
  if (toggle) {
    toggle.addEventListener("click", function () {
      const current = html.getAttribute("data-theme");
      const next = current === "dark" ? "light" : "dark";
      applyTheme(next);
      localStorage.setItem(STORAGE_KEY, next);
    });
  }

  // React to OS theme changes (only if no saved preference)
  window
    .matchMedia("(prefers-color-scheme: light)")
    .addEventListener("change", function (e) {
      if (!localStorage.getItem(STORAGE_KEY)) {
        applyTheme(e.matches ? "light" : "dark");
      }
    });
})();
