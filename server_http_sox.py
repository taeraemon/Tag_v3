from flask import Flask, request

app = Flask(__name__)

# POST 요청을 받을 '/receive' 엔드포인트
@app.route('/receive', methods=['POST'])
def receive_data():
    tag = request.form.get('tag')
    sig = request.form.get('sig')
    
    print(f"Received tag: {tag}")
    print(f"Received sig: {sig}")
    
    return 'Data received successfully', 200

# 기본 경로에 대한 라우트 추가 (잘못된 경로 확인용)
@app.route('/', methods=['POST'])
def catch_all():
    print("Received request at root ('/') endpoint")
    return 'Invalid URL, please use /receive', 404

# 모든 경로에 대해 라우트 추가 (기타 경로에서 요청을 받았는지 확인용)
@app.route('/<path:path>', methods=['POST'])
def catch_all_other(path):
    print(f"Received request at /{path} endpoint")
    return f'Invalid URL, use /receive instead of /{path}', 404

if __name__ == '__main__':
    # 서버 시작 (포트는 필요에 맞게 변경 가능)
    app.run(host='0.0.0.0', port=55000)
