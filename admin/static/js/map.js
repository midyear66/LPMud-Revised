// Interactive Leaflet map viewer for LPmud world map
(function () {
  var container = document.getElementById("map");
  var hasJson = container && container.getAttribute("data-has-json") === "true";

  var map = L.map("map", {
    crs: L.CRS.Simple,
    minZoom: -2,
    maxZoom: 4,
    zoomSnap: 0.25,
    attributionControl: false,
  });

  // Room markers indexed by room id for navigation
  var roomMarkers = {};
  var allRooms = {};

  function initPngFallback() {
    var W = 7200, H = 5400;
    var bounds = [[0, 0], [H, W]];
    L.imageOverlay("/map/image", bounds).addTo(map);
    map.fitBounds(bounds);
    setupResetView(bounds);
  }

  function setupResetView(bounds) {
    var resetBtn = document.getElementById("reset-view");
    if (resetBtn) {
      resetBtn.addEventListener("click", function () {
        map.fitBounds(bounds);
      });
    }
  }

  // Convert PNG y-down coords to Leaflet y-up coords
  function toLat(canvasH, y) {
    return canvasH - y;
  }

  function initInteractiveMap(data) {
    var cw = data.canvas.width;
    var ch = data.canvas.height;
    var bounds = [[0, 0], [ch, cw]];

    allRooms = data.rooms;

    // Dark background
    container.style.background = "rgb(25, 25, 35)";

    // Region backgrounds
    var regionKeys = Object.keys(data.regions);
    for (var r = 0; r < regionKeys.length; r++) {
      var rk = regionKeys[r];
      var reg = data.regions[rk];
      var bx = reg.box[0], by = reg.box[1], bw = reg.box[2], bh = reg.box[3];
      var fill = reg.fill;
      var border = reg.border;

      // Region rectangle
      var rBounds = [
        [toLat(ch, by + bh), bx],
        [toLat(ch, by), bx + bw]
      ];
      L.rectangle(rBounds, {
        color: "rgb(" + border[0] + "," + border[1] + "," + border[2] + ")",
        weight: 1,
        fillColor: "rgb(" + fill[0] + "," + fill[1] + "," + fill[2] + ")",
        fillOpacity: 0.07,
        interactive: false,
      }).addTo(map);

      // Region label at top-left of box
      var labelLat = toLat(ch, by) + 5;
      var labelLng = bx + 10;
      L.marker([labelLat, labelLng], {
        icon: L.divIcon({
          className: "region-label",
          html: '<span style="color:rgba(' + fill[0] + ',' + fill[1] + ',' + fill[2] + ',0.55);font-size:16px;font-weight:700;white-space:nowrap;">' + reg.displayName + '</span>',
          iconSize: null,
          iconAnchor: [0, 0],
        }),
        interactive: false,
      }).addTo(map);
    }

    // Edges
    for (var e = 0; e < data.edges.length; e++) {
      var edge = data.edges[e];
      var wp = edge.waypoints;
      var latlngs = [];
      for (var p = 0; p < wp.length; p++) {
        latlngs.push([toLat(ch, wp[p][1]), wp[p][0]]);
      }
      var lineOpts = { interactive: false, weight: 2 };
      if (edge.type === "normal") {
        lineOpts.color = "rgb(140,140,160)";
        lineOpts.opacity = 0.8;
      } else if (edge.type === "long") {
        lineOpts.color = "rgb(150,150,170)";
        lineOpts.opacity = 0.5;
        lineOpts.dashArray = "8,6";
      } else if (edge.type === "vertical") {
        lineOpts.color = "rgb(160,100,200)";
        lineOpts.opacity = 0.8;
        lineOpts.dashArray = "4,4";
      }
      L.polyline(latlngs, lineOpts).addTo(map);
    }

    // Room boxes
    var roomIds = Object.keys(data.rooms);
    for (var i = 0; i < roomIds.length; i++) {
      var id = roomIds[i];
      var room = data.rooms[id];
      var rx = room.x, ry = room.y;
      var lw = room.labelWidth;
      var padX = 6, padY = 3;
      var halfW = (lw / 2) + padX;
      var halfH = 10 + padY;
      var region = data.regions[room.region] || data.regions["special"];
      var rfill = region ? region.fill : [180, 180, 190];
      var rborder = region ? region.border : [130, 130, 140];

      var roomBounds = [
        [toLat(ch, ry + halfH), rx - halfW],
        [toLat(ch, ry - halfH), rx + halfW]
      ];

      var marker = L.rectangle(roomBounds, {
        color: "rgb(" + rborder[0] + "," + rborder[1] + "," + rborder[2] + ")",
        weight: 1,
        fillColor: "rgb(" + rfill[0] + "," + rfill[1] + "," + rfill[2] + ")",
        fillOpacity: 0.85,
      });

      marker.bindTooltip(room.label, {
        direction: "top",
        offset: [0, -halfH],
        className: "room-tooltip",
      });

      // Build popup content with closure
      (function (roomId, roomData, roomMarker) {
        roomMarker.on("click", function () {
          var html = '<div class="room-popup">';
          html += '<div class="room-popup-title">' + escapeHtml(roomData.label) + '</div>';
          html += '<div class="room-popup-info">' + escapeHtml(roomData.file) + '</div>';
          var regName = data.regions[roomData.region] ? data.regions[roomData.region].displayName : roomData.region;
          html += '<div class="room-popup-info">Region: ' + escapeHtml(regName) + '</div>';
          var exits = roomData.exits;
          var exitKeys = Object.keys(exits);
          if (exitKeys.length > 0) {
            html += '<div class="room-popup-exits">Exits:';
            for (var ek = 0; ek < exitKeys.length; ek++) {
              var dir = exitKeys[ek];
              var dest = exits[dir];
              var destLabel = data.rooms[dest] ? data.rooms[dest].label : dest;
              html += ' <a href="#" class="exit-link" data-room="' + escapeAttr(dest) + '">' + escapeHtml(dir) + '</a>';
            }
            html += '</div>';
          }
          html += '</div>';

          roomMarker.unbindPopup();
          roomMarker.bindPopup(html, { maxWidth: 300, className: "room-popup-container" }).openPopup();
        });
      })(id, room, marker);

      // Room label as divIcon centered on the room
      var textColor = region ? region.text || [30, 30, 40] : [30, 30, 40];
      L.marker([toLat(ch, ry), rx], {
        icon: L.divIcon({
          className: "room-label-icon",
          html: '<span style="color:rgb(' + textColor[0] + ',' + textColor[1] + ',' + textColor[2] + ')">' + escapeHtml(room.label) + '</span>',
          iconSize: [lw + padX * 2, halfH * 2],
          iconAnchor: [(lw + padX * 2) / 2, halfH],
        }),
        interactive: false,
      }).addTo(map);

      marker.addTo(map);
      roomMarkers[id] = marker;
    }

    // Delegated handler for exit links inside room popups
    container.addEventListener("click", function (ev) {
      var link = ev.target.closest(".exit-link");
      if (link) {
        ev.preventDefault();
        var targetId = link.getAttribute("data-room");
        navigateToRoom(targetId, ch);
      }
    });

    map.fitBounds(bounds);
    setupResetView(bounds);
    setupSearch(data.rooms, ch);
  }

  function navigateToRoom(roomId, canvasH) {
    var marker = roomMarkers[roomId];
    var room = allRooms[roomId];
    if (marker && room) {
      map.closePopup();
      map.setView([toLat(canvasH, room.y), room.x], 1, { animate: true });
      setTimeout(function () {
        marker.fire("click");
      }, 300);
    }
  }

  function setupSearch(rooms, canvasH) {
    var input = document.getElementById("map-search");
    var resultsDiv = document.getElementById("map-search-results");
    if (!input || !resultsDiv) return;

    var roomIds = Object.keys(rooms);

    input.addEventListener("input", function () {
      var query = input.value.toLowerCase().trim();
      resultsDiv.innerHTML = "";
      if (query.length < 2) {
        resultsDiv.style.display = "none";
        return;
      }
      var matches = [];
      for (var i = 0; i < roomIds.length && matches.length < 10; i++) {
        var id = roomIds[i];
        var room = rooms[id];
        if (id.toLowerCase().indexOf(query) !== -1 ||
            room.label.toLowerCase().indexOf(query) !== -1) {
          matches.push({ id: id, label: room.label });
        }
      }
      if (matches.length === 0) {
        resultsDiv.style.display = "none";
        return;
      }
      resultsDiv.style.display = "block";
      for (var m = 0; m < matches.length; m++) {
        var div = document.createElement("div");
        div.className = "map-search-item";
        div.textContent = matches[m].label + " (" + matches[m].id + ")";
        div.setAttribute("data-room", matches[m].id);
        div.addEventListener("click", function () {
          var targetId = this.getAttribute("data-room");
          navigateToRoom(targetId, canvasH);
          input.value = "";
          resultsDiv.style.display = "none";
        });
        resultsDiv.appendChild(div);
      }
    });

    // Close results on outside click
    document.addEventListener("click", function (ev) {
      if (!input.contains(ev.target) && !resultsDiv.contains(ev.target)) {
        resultsDiv.style.display = "none";
      }
    });
  }

  function escapeHtml(str) {
    var div = document.createElement("div");
    div.appendChild(document.createTextNode(str));
    return div.innerHTML;
  }

  function escapeAttr(str) {
    return str.replace(/&/g, "&amp;").replace(/"/g, "&quot;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  }

  // Load data or fall back to PNG
  if (hasJson) {
    fetch("/map/data")
      .then(function (resp) {
        if (!resp.ok) throw new Error("No JSON data");
        return resp.json();
      })
      .then(function (data) {
        initInteractiveMap(data);
      })
      .catch(function () {
        initPngFallback();
      });
  } else {
    initPngFallback();
  }
})();
