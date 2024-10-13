from django.http import JsonResponse

# Create your views here.

def index(request):
    if request.method == "GET":
        data = {
            "targetAngle": [45, 90, 135, 180, 45], # CHANGE
            "assistedAngle": [30, 60, 90, 120, 30] # CHANGE
        }
        return JsonResponse(data, status=200)
    
    return JsonResponse({"error": "Invalid request method"}, status=405)