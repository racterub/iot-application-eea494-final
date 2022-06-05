#!/usr/bin/env python3

from flask import Flask, request, url_for, render_template
import httpx


app = Flask(__name__)
devices = {"192.168.88.139": ""}

@app.route("/")
async def index():
    global devices
    for device in devices:
        async with httpx.AsyncClient() as client:
            try:
                response = await client.get(f"http://{device}/status")
                devices[device] = response.json()
            except:
                pass
    return render_template("home.jinja", device_count=len(devices), devices=devices.keys())


@app.route("/device/<string:device>")
async def device(device):
    global devices
    if device in devices:
        async with httpx.AsyncClient() as client:
            if request.args.get("fan") == "1":
                try:
                    r = await client.get(f"http://{device}/fanOn")
                    print(r.text)
                except Exception as e:
                    print(e)
            elif request.args.get("fan") == "0":
                try:
                    r = await client.get(f"http://{device}/fanOff")
                    print(r.text)
                except Exception as e:
                    print(e)
            if request.args.get("led") == "1":
                try:
                    r = await client.get(f"http://{device}/ledOn")
                    print(r.text)
                except Exception as e:
                    print(e)
            elif request.args.get("led") == "0":
                try:
                    r = await client.get(f"http://{device}/ledOff")
                    print(r.text)
                except Exception as e:
                    print(e)
            try:
                response = await client.get(f"http://{device}/status")
                devices[device] = response.json()
                devices[device]["temperature"] = round(devices.get(device).get("temperature"), 2)
                devices[device]["humidity"] = round(devices.get(device).get("humidity"), 2)
                print(devices)
            except Exception as e:
                print(e)
        return render_template("device.jinja", device=device, data=devices[device])
    else:
        return render_template("device.jinja", device=device, data=None)


if __name__ == "__main__":
    app.run("0.0.0.0", port=8080, debug=True)