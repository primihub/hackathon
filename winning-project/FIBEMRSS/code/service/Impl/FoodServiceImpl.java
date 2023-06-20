package com.ischoolbar.programmer.service.Impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.ischoolbar.programmer.dao.FoodDao;
import com.ischoolbar.programmer.entity.Food;
import com.ischoolbar.programmer.service.FoodService;

@Service
public class FoodServiceImpl implements FoodService {

	@Autowired
	private FoodDao foodDao;
	@Override
	public int add(Food food) {
		// TODO Auto-generated method stub
		return foodDao.add(food);
	}

	@Override
	public int edit(Food food) {
		// TODO Auto-generated method stub
		return foodDao.edit(food);
	}

	@Override
	public List<Food> findList(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return foodDao.findList(queryMap);
	}

	@Override
	public Integer getTotal(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return foodDao.getTotal(queryMap);
	}

	@Override
	public int delete(Long id) {
		// TODO Auto-generated method stub
		return foodDao.delete(id);
	}

	@Override
	public Food find(Long id) {
		// TODO Auto-generated method stub
		return foodDao.find(id);
	}

	@Override
	public int updateSels(Long id, Long num) {
		// TODO Auto-generated method stub
		return foodDao.updateSels(id, num);
	}

}
