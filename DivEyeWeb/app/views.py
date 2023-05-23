import json
from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render, HttpResponse, HttpResponseRedirect
from django.http import JsonResponse
from app.models import *
import requests


# Create your views here.
def node_status(req):
    node_statuses = nodestatus.objects.all()
    return render(req, "nodeStatus.html", {'node_statuses': node_statuses})


def index(req):
    return render(req, "index.html")


def colors(req):
    return HttpResponseRedirect('https://mycolor.space/?hex=%232C73D2&sub=1')


@csrf_exempt
def frqInfo(req):
    param = json.loads(req.body.decode('utf-8'))
    node_name = param["nodeName"]
    ipaddr = nodestatus.objects.get(name=node_name).addr
    response = requests.get("http://" + ipaddr + ":8007/FrqInfoGet")

    string = response.text
    data = {}
    for it in string.split():
        x = it.split(':')
        data[x[0]] = x[1]
    return JsonResponse(data)


@csrf_exempt
def frqDB(req):
    param = json.loads(req.body.decode('utf-8'))
    node_name = param['nodeName']
    cpu = cpuinfo.objects.filter(node=node_name).order_by('-time')[:30]
    mem = meminfo.objects.filter(node=node_name).order_by('-time')[:30]
    data = []
    cpudata = []
    memdata = []
    if len(cpu) < len(mem):
        length = len(cpu)
    else:
        length = len(mem)
    for i in range(length - 1):
        data.append(i)
        cpudata.append(cpu[i].cpuuse)
        memdata.append(mem[i].freeram / mem[i].totalram)
        # data[i] = {'time': cpu[i].time, 'cpuuse': cpu[i].cpuuse, 'totalram': mem[i].totalram, 'freeram': mem[i].freeram}
    option = {
        'tooltip': {
            'trigger': 'axis',
            'axisPointer': {'type': 'cross'}
        },
        'legend': {},
        'xAxis': [
            {
                'type': 'category',
                'axisTick': {
                    'alignWithLabel': True
                },
                'data': data
            }
        ],
        'yAxis': [
            {
                'type': 'value'
            },
            {
                'type': 'value',
            }
        ],
        'series': [
            {
                'name': 'cpu',
                'type': 'line',
                'yAxisIndex': 0,
                'data': cpudata,
                'position': 'right'
            },
            {
                'name': 'mem',
                'type': 'line',
                'yAxisIndex': 1,
                'position': 'left',
                'data': memdata
            }
        ]
    }
    return JsonResponse(option)


@csrf_exempt
def frq_gpu(req):
    param = json.loads(req.body.decode('utf-8'))
    node_name = param["nodeName"]
    ipaddr = nodestatus.objects.get(name=node_name).addr
    response = requests.get("http://" + ipaddr + ":8007/FrqGPUInfoGet")
    gpu_info = response.text
    name = []
    use = []
    for it in gpu_info.split():
        x = it.split(':')
        name.append(x[0])
        use.append(x[1])
    option = {
        'xAxis': {},
        'yAxis': {
            'type': 'category',
            'data': name,
        },
        'series': [
            {
                'type': 'bar',
                'name': 'GPU',
                'data': use,
                'stack': 'y',
                'barGap': '10%',
                'barCategoryGap': '10%'
            }
        ],
        'legend': {
            'show': True
        },
    }
    return JsonResponse(option)


@csrf_exempt
def user_status(req):
    node_status_db = nodestatus.objects.all()
    user_list_db = userinfo.objects.all()
    user_list = []
    node_list = []
    for ns in node_status_db:
        node_list.append(ns.name)
    for ul in user_list_db:
        user_list.append(ul.name)

    return render(req, "userStatus.html", {'user_list': user_list, 'node_list': node_list})


@csrf_exempt
def user_use_select(req):
    param = json.loads(req.body.decode('utf-8'))
    node_name = param['node']
    user_name = param['user']
    print(node_name, user_name)
    if node_name == 'All':
        node_status_db = nodestatus.objects.all()
    else:
        node_status_db = nodestatus.objects.filter(name=node_name)

    if user_name == 'All':
        user_list_db = userinfo.objects.all()
    else:
        user_list_db = userinfo.objects.filter(name=user_name)

    data = {}
    axis = []
    cpu = []
    mem = []
    for ns in node_status_db:
        for ul in user_list_db:
            user_use_db = useruse.objects.filter(node=ns.name).filter(uid=ul.id).order_by('-time')[:30]
            cpu_sum = mem_sum = 0.0
            for uu in user_use_db:
                cpu_sum += uu.cpuuse
                mem_sum += uu.ramuse
            length = len(user_use_db)
            if length != 0:
                data[ns.name + ':' + ul.name] = [cpu_sum / length, cpu_sum / length]
                axis.append(ns.name + ':' + ul.name)
                cpu.append(cpu_sum / length)
                mem.append(cpu_sum / length)
        data[ns.name + ':' + 'test'] = [10, 300]
        axis.append(ns.name + ':' + 'test')
        cpu.append(10)
        mem.append(30)

    option = {
        'xAxis': {},
        'yAxis': {
            'type': 'category',
            'data': axis,
        },
        'series': [
            {
                'type': 'bar',
                'name': 'CPU',
                'data': cpu,
                'stack': 'y'
            },
            {
                'type': 'bar',
                'name': 'MEM',
                'data': mem,
                'stack': 'y'
            }
        ],
        'legend': {
            'show': True
        },
    }
    return JsonResponse(option)
