<template>
  <div v-bind="$attrs" class="tip">{{text}}</div>
</template>

<script>
export default {
  name: 'ToolTip',
  props:{
    text:{
      type: String,
      default:'报名即将开始'
    }
  }
}
</script>

<style lang="scss" scoped>
.tip{
  position: fixed;
  top: 20%;
  left: 50%;
  transform: translate3d(-50%,0,0);
  background: rgba(0,0,0,.8);
  border-radius: 10px;
  padding: 10px 50px;
  color: #fff;
}
</style>