mergeInto(LibraryManager.library, {
  emscripten_async_wget3_data: function(url, request, data, dataSize, additionalHeader, arg, free, onload, onerror, onprogress) {
    var _url = UTF8ToString(url);
    var _request = UTF8ToString(request);

    // copy post payload to some other place in heap, since the C++ may free that memory
    // for example, KIS proxy all requests and carry them out on next frames 
    // which leads the garbage to be put in the original data for POST requests
    var dataPtr = Module._malloc(dataSize);
    var _param = new Uint8Array(Module.HEAPU8.buffer, dataPtr, dataSize);
    _param.set(new Uint8Array(Module.HEAPU8.buffer, data, dataSize));

    var http = new XMLHttpRequest();
    http.open(_request, _url, true);
    http.responseType = 'arraybuffer';

    var handle = Browser.getNextWgetRequestHandle();

    // LOAD
    http.onload = function http_onload(e) {
      if (http.status == 200 || _url.substr(0,4).toLowerCase() != "http") {
        var byteArray = new Uint8Array(http.response);
        var buffer = _malloc(byteArray.length);
        HEAPU8.set(byteArray, buffer);
        if (onload) Module['dynCall_viiii'](onload, handle, arg, buffer, byteArray.length);
        if (free) _free(buffer);
      } else {
        if (onerror) Module['dynCall_viiii'](onerror, handle, arg, http.status, http.statusText);
      }
      delete Browser.wgetRequests[handle];
      Module._free(_param.byteOffset);
    };

    // ERROR
    http.onerror = function http_onerror(e) {
      if (onerror) {
        Module['dynCall_viiii'](onerror, handle, arg, http.status, http.statusText);
      }
      delete Browser.wgetRequests[handle];
      Module._free(_param.byteOffset);
    };

    // PROGRESS
    http.onprogress = function http_onprogress(e) {
      if (onprogress) Module['dynCall_viiii'](onprogress, handle, arg, e.loaded, e.lengthComputable || e.lengthComputable === undefined ? e.total : 0);
    };

    // ABORT
    http.onabort = function http_onabort(e) {
      delete Browser.wgetRequests[handle];
      Module._free(_param.byteOffset);
    };

    // Useful because the browser can limit the number of redirection
    try {
      if (http.channel instanceof Ci.nsIHttpChannel)
      http.channel.redirectionLimit = 0;
    } catch (ex) { /* whatever */ }

    try {
      var additionalHeaderObject = JSON.parse(UTF8ToString(additionalHeader));
      for (var entry in additionalHeaderObject) {
        http.setRequestHeader(entry, additionalHeaderObject[entry]);
      }
    } catch (ex) { }

    if (_request == "POST") {
      //Send the proper header information along with the request
      http.send(_param);
    } else {
      http.send(null);
    }

    Browser.wgetRequests[handle] = http;

    return handle;
  }
});


mergeInto(LibraryManager.library, {
copyTextToClipboard:function (text) {
  var textArea = document.createElement("textarea");

  //
  // *** This styling is an extra step which is likely not required. ***
  //
  // Why is it here? To ensure:
  // 1. the element is able to have focus and selection.
  // 2. if element was to flash render it has minimal visual impact.
  // 3. less flakyness with selection and copying which **might** occur if
  //    the textarea element is not visible.
  //
  // The likelihood is the element won't even render, not even a flash,
  // so some of these are just precautions. However in IE the element
  // is visible whilst the popup box asking the user for permission for
  // the web page to copy to the clipboard.
  //

  // Place in top-left corner of screen regardless of scroll position.
  textArea.style.position = 'fixed';
  textArea.style.top = 0;
  textArea.style.left = 0;

  // Ensure it has a small width and height. Setting to 1px / 1em
  // doesn't work as this gives a negative w/h on some browsers.
  textArea.style.width = '2em';
  textArea.style.height = '2em';

  // We don't need padding, reducing the size if it does flash render.
  textArea.style.padding = 0;

  // Clean up any borders.
  textArea.style.border = 'none';
  textArea.style.outline = 'none';
  textArea.style.boxShadow = 'none';

  // Avoid flash of white box if rendered for any reason.
  textArea.style.background = 'transparent';


  textArea.value = UTF8ToString(text);

  document.body.appendChild(textArea);

  textArea.select();

  try {
    var successful = document.execCommand('copy');
    var msg = successful ? 'successful' : 'unsuccessful';
    console.log('Copying text command was ' + msg);
  } catch (err) {
    console.log('Oops, unable to copy');
  }

  document.body.removeChild(textArea);
}
});
