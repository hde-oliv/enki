import requests
import json
from jsonschema import validate
from jsonschema import Draft6Validator

baseurl = "http://localhost:8080/"

schema = {
    "$schema": "https://json-schema.org/schema#",

    "type" : "object",
    "properties" : {
        "intra_id" : {"type" : "number"},
        "login" : {"type" : "string"},
        "name" : {"type" : "string"},
        "bocal" : {"type" : "boolean"},
        "specialization" : {"type" : "boolean"},
        "correction_points" : {"type" : "number"},
        "wallet" : {"type" : "number"},
        "concluded_projects" : {"type" : "number"},
        "day_started_on_42" : {"type" : "string"},
        "days_wasted_on_retries" : {"type" : "number"},
        "stardew_valley_coeficient" : {"type" : "number"},
        "42_level" : {"type" : "number"},
    },
}

def test_get_200_on_no_route():
    response = requests.get(baseurl)
    assert response.status_code == 200 # cause redirect

def test_get_200_on_docs_route():
    response = requests.get(baseurl + "docs")
    assert response.status_code == 200 # cause redirect

def test_get_200_on_docs_route_2():
    response = requests.get(baseurl + "docs/")
    assert response.status_code == 200 # cause redirect

def test_get_200_on_users_route():
    response = requests.get(baseurl + "api/v1/hde-oliv")
    assert response.status_code == 200

def test_post_405_on_users_route():
    response = requests.post(baseurl + "api/v1/hde-oliv")
    assert response.status_code == 405

def test_get_404_on_invalid_route():
    response = requests.get(baseurl + "api/vvv1/hde-oliv")
    assert response.status_code == 404

def test_get_404_on_invalid_route_2():
    response = requests.get(baseurl + "api/v1/hde-olivasdjfalsdkjfl")
    assert response.status_code == 404

def test_get_hde_oliv_parse_json_resonse_schema():
    response = requests.get(baseurl + "api/v1/hde-oliv")
    assert response.status_code == 200
    assert response.headers["Content-Type"] == "application/json; charset=utf-8"
    resp_body = response.json()
    validate(instance=resp_body, schema=schema)
