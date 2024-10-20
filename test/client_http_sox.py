import requests

url = "http://localhost:55000/receive"
data = {
    'tag': 'abcd',
    'sig': '1725575859953|450|3|24_1550_0_324032[-32/-44],2_1550_2_0[-52/-74],122_1565_2_0[-82/-94]|3|e848b812e7a7[-70],909f33a38816[-75],167f6730787a[-87]'
}

response = requests.post(url, data=data)

print(f"Status Code: {response.status_code}")
print(f"Response Text: {response.text}")
