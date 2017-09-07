#include "home.html.h"

PGM_P homepage PROGMEM = "<!DOCTYPE html>\
<html>\
  <head>\
    <style>\
      .main {\
        height: 100vh;\
        display: flex;\
        flex-direction: column;\
        align-items: center;\
        justify-content: center;\
      }\
    </style>\
  </head>\
  <body>\
    <script>\
      function onSubmit() {\
        let ssid = document.querySelector('#config-ssid').value;\
        let pass = document.querySelector('#config-pass').value;\
        fetch('/config?ssid=' + encodeURI(ssid) + '&pass=' + encodeURI(pass))\
        .then((response) => response.json())\
        .then(() => {\
          alert('Saved!');\
        }).catch(() => {\
          alert('Could not save the network configuration!');\
        });\
        return false;\
      }\
    </script>\
    <div class='main'>\
      <form id='config-form' onsubmit='return onSubmit()'>\
        <fieldset>\
          <legend>Network configuration:</legend>\
          <div><input id='config-ssid' type='text' name='ssid' placeholder='Network SSID' pattern='.+' required></div>\
          <div><input id='config-pass' type='password' name='pass' placeholder='p4$$w0rd' pattern='.+' required></div>\
          <div><input type='submit' value='Save'></div>\
        </fieldset>\
      </form>\
    </div>\
  </body>\
</html>";
