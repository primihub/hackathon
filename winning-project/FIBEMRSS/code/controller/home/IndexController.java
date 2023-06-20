package com.ischoolbar.programmer.controller.home;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.ModelAndView;

import com.ischoolbar.programmer.entity.Food;
import com.ischoolbar.programmer.entity.FoodCategory;
import com.ischoolbar.programmer.service.FoodCategoryService;



/**
 * 前台控制器
 * @author lenovo
 *
 */

@RequestMapping("/home/index")
@Controller
public class IndexController {

	@Autowired
	private FoodCategoryService foodCategoryService;
	
	/**
	 * 前台首页
	 * @param model
	 * @return
	 */
	@RequestMapping(value="/index",method=RequestMethod.GET)
	public ModelAndView index(ModelAndView model){
		model.setViewName("home/index/index");
		return model;
	}
	
	/**
	 * 获取所有的菜品信息
	 * @return
	 */
	@RequestMapping(value="/get_food_list",method=RequestMethod.POST)
	@ResponseBody
	public Map<String,Object> getFoodList(){
		Map<String, Object> ret = new HashMap<String, Object>();
		List<FoodCategory> findAll = foodCategoryService.findAll();
		Map<String,List<Food>> content = new HashMap<String, List<Food>>();
		for(FoodCategory foodCategory:findAll){
			content.put(foodCategory.getName(), foodCategory.getFoodList());
		}
		ret.put("type", "success");
		ret.put("content", content);
		return ret;
	}

}
