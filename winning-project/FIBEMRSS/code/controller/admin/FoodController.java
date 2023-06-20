package com.ischoolbar.programmer.controller.admin;

import java.util.HashMap;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.ModelAndView;

import com.ischoolbar.programmer.entity.Food;
import com.ischoolbar.programmer.page.amdin.Page;
import com.ischoolbar.programmer.service.FoodCategoryService;
import com.ischoolbar.programmer.service.FoodService;
import com.mysql.jdbc.StringUtils;

@RequestMapping("/admin/food")
@Controller
public class FoodController {

	@Autowired
	private FoodCategoryService foodCategoryService;
	
	@Autowired
	private FoodService foodService;
	
	/**
	 * 菜品信息列表页面
	 * @param model
	 * @return
	 */
	@RequestMapping(value="/list",method=RequestMethod.GET)
	public ModelAndView list(ModelAndView model){
		model.setViewName("food/list");
		Map<String, Object> queryMap = new HashMap<String, Object>();
		queryMap.put("offset", 0);
		queryMap.put("pageSize", 9999);
		model.addObject("foodCategoryList", foodCategoryService.findList(queryMap));
		return model;
	}
	
	/**
	 * 模糊分页查询菜品信息
	 * @return
	 */
	@RequestMapping(value="/list",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, Object> getList(
			@RequestParam(name="name",defaultValue="") String name,
			@RequestParam(name="categoryId",required=false) Long categoryId,
			Page page
			){
		Map<String, Object> ret = new HashMap<String, Object>();
		Map<String, Object> queryMap = new HashMap<String, Object>();
		queryMap.put("name", name);
		if(categoryId != null && categoryId.longValue() != -1){
			queryMap.put("categoryId", categoryId);
		}
		queryMap.put("offset", page.getOffset());
		queryMap.put("pageSize", page.getRows());
		ret.put("rows", foodService.findList(queryMap));
		ret.put("total", foodService.getTotal(queryMap));
		return ret;
	}
	
	/**
	 * 菜品信息添加
	 * @param food
	 * @return
	 */
	@RequestMapping(value="/add",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, String> add(Food food){
		Map<String, String> ret = new HashMap<String, String>();
		if(food == null){
			ret.put("type", "error");
			ret.put("msg", "请填写正确的信息!");
			return ret;
		}
		if(StringUtils.isNullOrEmpty(food.getName())){
			ret.put("type", "error");
			ret.put("msg", "菜品信息名称不能为空!");
			return ret;
		}
		if(food.getCategoryId() == null){
			ret.put("type", "error");
			ret.put("msg", "请选择所属分类!");
			return ret;
		}
		if(foodService.add(food) <= 0){
			ret.put("type", "error");
			ret.put("msg", "菜品信息添加失败，请联系管理员!");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "添加成功！");
		return ret;
	}
	
	/**
	 * 菜品信息编辑
	 * @param food
	 * @return
	 */
	@RequestMapping(value="/edit",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, String> edit(Food food){
		Map<String, String> ret = new HashMap<String, String>();
		if(food == null){
			ret.put("type", "error");
			ret.put("msg", "请填写正确的信息!");
			return ret;
		}
		if(StringUtils.isNullOrEmpty(food.getName())){
			ret.put("type", "error");
			ret.put("msg", "菜品信息名称不能为空!");
			return ret;
		}
		if(food.getCategoryId() == null){
			ret.put("type", "error");
			ret.put("msg", "请选择所属分类!");
			return ret;
		}
		if(foodService.edit(food) <= 0){
			ret.put("type", "error");
			ret.put("msg", "菜品信息修改失败，请联系管理员!");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "添加成功！");
		return ret;
	}
	
	/**
	 * 菜品信息删除
	 * @param id
	 * @return
	 */
	@RequestMapping(value="/delete",method=RequestMethod.POST)
	@ResponseBody
	public Map<String, String> delete(Long id){
		Map<String, String> ret = new HashMap<String, String>();
		if(id == null){
			ret.put("type", "error");
			ret.put("msg", "请选择要删除的信息!");
			return ret;
		}
		try {
			if(foodService.delete(id) <= 0){
				ret.put("type", "error");
				ret.put("msg", "菜品信息删除失败，请联系管理员!");
				return ret;
			}
		} catch (Exception e) {
			// TODO: handle exception
			ret.put("type", "error");
			ret.put("msg", "该菜品信息下存在订单信息，请先删除相关订单信息!");
			return ret;
		}
		ret.put("type", "success");
		ret.put("msg", "删除成功！");
		return ret;
	}
}
