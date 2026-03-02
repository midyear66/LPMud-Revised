// Leaflet map viewer for LPmud world map
(function () {
  // Map image dimensions (from generate-map.py)
  var W = 4600;
  var H = 3450;

  var map = L.map("map", {
    crs: L.CRS.Simple,
    minZoom: -2,
    maxZoom: 3,
    zoomSnap: 0.25,
    attributionControl: false,
  });

  // Image bounds in pixel coordinates (y-axis inverted for Leaflet)
  var bounds = [
    [0, 0],
    [H, W],
  ];

  L.imageOverlay("/map/image", bounds).addTo(map);
  map.fitBounds(bounds);

  // Reset view button
  var resetBtn = document.getElementById("reset-view");
  if (resetBtn) {
    resetBtn.addEventListener("click", function () {
      map.fitBounds(bounds);
    });
  }
})();
