>>> import requests
>>> response = requests.get('http://0.0.0.0:8000/static/Curve.html')
>>> print( response )
>>> response.__dict__
>>> response = requests.post('http://0.0.0.0:8000/GetCurvePoints', data = { 'Approach': 'EvaluateCatmullRomSpline', 'ControlPoints': "[ [ 1, 0 ], [ 1, 1 ], [ 0, 1 ], [ 0, 0 ] ]", "SamplesPerCurve": 3, "Alpha": 0.1 } )
>>> print( response )
>>> response.__dict__
