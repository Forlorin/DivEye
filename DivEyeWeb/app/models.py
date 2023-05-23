from django.db import models
import django.contrib.postgres


# Create your models here.
class cpuinfo(models.Model):
    time = models.CharField()
    node = models.CharField()
    cpuuse = models.FloatField()

    class Meta:
        managed = False
        db_table = 'cpuinfo'


class meminfo(models.Model):
    time = models.CharField()
    node = models.CharField()
    totalram = models.FloatField()
    freeram = models.FloatField()

    class Meta:
        managed = False
        db_table = 'meminfo'


class nodestatus(models.Model):
    name = models.CharField()
    addr = models.CharField(primary_key=True)
    status = models.IntegerField()
    time = models.CharField()

    class Meta:
        managed = False
        db_table = 'nodestatus'


class userinfo(models.Model):
    id = models.IntegerField(primary_key=True)
    name = models.CharField()

    class Meta:
        managed = False
        db_table = 'userinfo'


class useruse(models.Model):
    time = models.CharField()
    node = models.CharField()
    uid = models.IntegerField()
    cpuuse = models.FloatField()
    ramuse = models.FloatField()

    class Meta:
        managed = False
        db_table = 'useruse'
